#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>

#define _BSD_SOURCE

#define PORT 2929

// *****************************************************
// Name: get_a_pointer
// Description: Gets a pointer to the physical address within /dev/mem
// *****************************************************
volatile unsigned int *get_a_pointer(unsigned int phys_addr)
{
    int mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    void *map_base = mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, phys_addr);
    volatile unsigned int *radio_base = (volatile unsigned int *)map_base;

    return (radio_base);
}

// *****************************************************
// Name: main
// Description: Main function of the program
// *****************************************************
int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("Incorrect number of arguments\r\n");
        return -1;
    }
    const char* ip_address = argv[1];
    unsigned int num_packets = atoi(argv[2]);

    int buffer[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;

    printf("Sending %u packets to address %s...\r\n", num_packets, ip_address);
    // Create socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip_address);
    servaddr.sin_port = htons(PORT);

    for (unsigned int i = 0; i < num_packets; i++)
    {
        sendto(sockfd, buffer, sizeof(buffer), MSG_CONFIRM, (const struct sockaddr*)&servaddr, sizeof(servaddr));
        buffer[0] += 1;
    }

    return 0;
}

// EOF
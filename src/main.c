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
#include <pthread.h>
#include <semaphore.h>

#define _BSD_SOURCE

#define RADIO_ADC_PINC_OFFSET 0
#define RADIO_TUNER_PINC_OFFSET 1
#define RADIO_CONTROL_OFFSET 2
#define RADIO_TIMER_OFFSET 3
#define RADIO_ADDRESS 0x43c00000

#define FIFO_RD_CNT_OFFSET 0
#define FIFO_DATA_OFFSET 1
#define FIFO_READY_OFFSET 2
#define FIFO_ADDRESS 0x43c10000

#define PORT 25344

// Global variables shared between two threads
int sockfd;
struct sockaddr_in servaddr;

struct Packet
{
    unsigned short counter;
    int data[256];
} packet;

int quit;
int stream;
pthread_mutex_t lock;

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
// Name: print_menu
// Description: Prints user menu to screen
// *****************************************************
void print_menu(void)
{
    printf("Menu Options:\r\n");
    printf("p - Print menu\r\n");
    printf("F <arg> - Set to frequency\r\n");
    printf("u - Step up frequency by 100 Hz\r\n");
    printf("U - Step up frequency by 1000 Hz\r\n");
    printf("d - Step down frequency by 100 Hz\r\n");
    printf("D - Step down frequency by 1000 Hz\r\n");
    printf("T <arg> - Tune to frequency\r\n");
    printf("s 1 - Enable UDP stream\r\n");
    printf("s 0 - Disable UDP stream\r\n");
    printf("e - Exit program\r\n");
}

// *****************************************************
// Name: write_pinc
// Description: Calculates phase increment based on frequency
// *****************************************************
unsigned int write_pinc(unsigned int freq)
{
    double phase_inc = (freq / 125000000.0) * 4294967296.0;
    unsigned int pinc = (unsigned int)phase_inc;
    printf("Set frequency to %d Hz, phase increment = %d\r\n", freq, pinc);

    return pinc;
}

// *****************************************************
// Name: stream_t
// Description: Reads the FIFO and streams UDP packets
// *****************************************************
void *stream_t(void *vargp)
{
    unsigned int rd_cnt = 0;
    volatile unsigned int *fifo_base = get_a_pointer(FIFO_ADDRESS);
    unsigned int words_read = 0;

    while(1)
    {
        rd_cnt = *(fifo_base + FIFO_RD_CNT_OFFSET);  
        for(unsigned int i = 0; i < rd_cnt; i++)
        {
            *(fifo_base + FIFO_READY_OFFSET) = 0x01;
            //int data = *(fifo_base + FIFO_DATA_OFFSET);
            //short i_data = (short)((data >> 16) & 0xffff); // Bits 15-0
            //short q_data = (short)(data & 0xffff); // Bits 31-16
            //packet.data[words_read] = ((int)q_data & 0xffff) | ((int)i_data << 16);
            packet.data[words_read] = *(fifo_base + FIFO_DATA_OFFSET);
            *(fifo_base + FIFO_READY_OFFSET) = 0x00;
            if(words_read == 255) break;
            words_read += 1;
        }
        if(words_read == 255)
        {
            pthread_mutex_lock(&lock);
            if(stream == 1)
            {
                sendto(sockfd, (struct Packet*)&packet, sizeof(packet), MSG_CONFIRM, (const struct sockaddr*)&servaddr, sizeof(servaddr));
                packet.counter += 1;
            }
            pthread_mutex_unlock(&lock);
            words_read = 0;
        }
        if(quit == 1)
        {
            break;
        }
    }
}

// *****************************************************
// Name: main
// Description: Main function of the program
// *****************************************************
int main(int argc, char **argv)
{
    if(argc != 2)
    {
        printf("Incorrect number of arguments, ./main <ip address> \r\n");
        return -1;
    }

    volatile unsigned int *radio_base = get_a_pointer(RADIO_ADDRESS);

    char key;
    int adc_freq = 0;
    int tuner_freq = 0;
    quit = 0;
    stream = 0;
    const char* ip_address = argv[1];

    pthread_t stream_thread;

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&packet, 0, sizeof(packet));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip_address);
    servaddr.sin_port = htons(PORT);

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Socket creation failed\r\n");
        return -1;
    }

    if(pthread_mutex_init(&lock, NULL) != 0)
    {
        perror("Mutex initialization failed\r\n");
        return -1;
    }

    if(pthread_create(&stream_thread, NULL, stream_t, NULL) < 0)
    {
        perror("Thread creation failed\r\n");
        return -1;
    }

    printf("Welcome to the Linux Software-Defined Radio\r\n");
    printf("Creator: Kevin Parlak\r\n");
    print_menu();

    while(1)
    {
        scanf("%c", &key);

        if(key == 'p')
        {
            print_menu();
        }
        else if(key == 'F')
        {
            scanf("%d", &adc_freq);
            *(radio_base + RADIO_ADC_PINC_OFFSET) = write_pinc(adc_freq);
        }
        else if(key == 'u')
        {
            adc_freq += 100;
            *(radio_base + RADIO_ADC_PINC_OFFSET) = write_pinc(adc_freq);
        }
        else if(key == 'U')
        {
            adc_freq += 1000;
            *(radio_base + RADIO_ADC_PINC_OFFSET) = write_pinc(adc_freq);
        }
        else if(key == 'd')
        {
            if(adc_freq > 100)
            {
                adc_freq -= 100;
                *(radio_base + RADIO_ADC_PINC_OFFSET) = write_pinc(adc_freq);
            }
            else
            {
                printf("Frequency below 0\r\n");
            }
        }
        else if(key == 'D')
        {
            if(adc_freq > 1000)
            {
                adc_freq -= 1000;
                *(radio_base + RADIO_ADC_PINC_OFFSET) = write_pinc(adc_freq);
            }
            else
            {
                printf("Frequency below 0\r\n");
            }
        }
        else if(key == 'T')
        {
            scanf("%d", &tuner_freq);
            *(radio_base + RADIO_TUNER_PINC_OFFSET) = write_pinc(tuner_freq);
        }
        else if(key == 's')
        {
            scanf("%d", &stream);
            if(stream == 0)
            {
                printf("UDP stream disabled\r\n");
            }
            else if(stream == 1)
            {
                printf("UDP stream enabled\r\n");
            }
        }
        else if(key == 'e')
        {
            quit = 1;
            *(radio_base + RADIO_ADC_PINC_OFFSET) = write_pinc(0);
            *(radio_base + RADIO_TUNER_PINC_OFFSET) = write_pinc(0);
            printf("Exiting...\r\n");
            break;
        }
    }

    // Shutdown thread and close socket
    pthread_join(stream_thread, NULL);
    pthread_exit(NULL);

    close(sockfd);

    return 0;
}

// EOF
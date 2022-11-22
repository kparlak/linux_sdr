#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#define _BSD_SOURCE

#define RADIO_TIMER_REG_OFFSET 3
#define RADIO_PERIPH_ADDRESS 0x43c00000

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
    if (argc < 2 || argc > 2)
    {
        printf("Incorrect number of arguments\r\n");
        return -1;
    }
    volatile unsigned int *periph_base = get_a_pointer(RADIO_PERIPH_ADDRESS);	
    unsigned int start_time;
    unsigned int stop_time;
    unsigned int num_samples = atoi(argv[1]);

    printf("Samples = %u\n", num_samples);
    start_time = *(periph_base + RADIO_TIMER_REG_OFFSET);

    for (int i = 0; i < num_samples; i++)
    {
        stop_time = *(periph_base + RADIO_TIMER_REG_OFFSET);
    }
    printf("Elapsed time in clocks = %u\n",stop_time-start_time);

    float throughput = (num_samples * (32.0 / 8.0) * (1 / 1000000.0)) / ((stop_time - start_time) * 0.000000008);
    printf("Estimated transfer throughput = %f Mbytes/sec\r\n", throughput);

    return 0;
}

// EOF
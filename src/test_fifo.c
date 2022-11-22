#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#define _BSD_SOURCE

#define FIFO_RD_CNT_REG_OFFSET 0
#define FIFO_DATA_REG_OFFSET 1
#define FIFO_READY_REG_OFFSET 2
#define FIFO_ADDRESS 0x43c10000

#define RADIO_ADC_PINC_OFFSET 0
#define RADIO_TUNER_PINC_OFFSET 1
#define RADIO_TIMER_REG_OFFSET 3
#define RADIO_ADDRESS 0x43c00000

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

    volatile unsigned int *radio_base = get_a_pointer(RADIO_ADDRESS);
    volatile unsigned int *fifo_base = get_a_pointer(FIFO_ADDRESS);

    double adc_pinc = (15000.0/125.0e6)*4294967296.0;
    double tuner_pinc = (10000.0/125.0e6)*4294967296.0;
    *(radio_base + RADIO_ADC_PINC_OFFSET) = (unsigned int)adc_pinc;
    *(radio_base + RADIO_TUNER_PINC_OFFSET) = (unsigned int)tuner_pinc;

    *(fifo_base + FIFO_READY_REG_OFFSET) = 0x00;

    int data = *(fifo_base + FIFO_DATA_REG_OFFSET);
    unsigned int rd_cnt = *(fifo_base + FIFO_RD_CNT_REG_OFFSET);
    unsigned int words_read = 0;
    unsigned int start_time = *(radio_base + RADIO_TIMER_REG_OFFSET);

    printf("Reading 4800000 samples...\r\n");
    do
    {
        rd_cnt = *(fifo_base + FIFO_RD_CNT_REG_OFFSET);
        for (unsigned int i = 0; i < rd_cnt; i++)
        {
            *(fifo_base + FIFO_READY_REG_OFFSET) = 0x01;
            data = *(fifo_base + FIFO_DATA_REG_OFFSET);
            *(fifo_base + FIFO_READY_REG_OFFSET) = 0x00;
            words_read += 1;
        }
    } while (words_read < 480000);

    unsigned int stop_time = *(radio_base + RADIO_TIMER_REG_OFFSET);
    printf("Finished, %f sec elapsed\r\n", ((stop_time - start_time) * 0.000000008));

    return 0;
}

// EOF
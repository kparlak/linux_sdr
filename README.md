# Linux Software-Defined Radio

EN.525.742 - System-on-a-Chip FPGA Design Laboratory project

This project was completed within the System-on-a-Chip FPGA Design Laboratory course at Johns Hopkins University. It constructs firmware to create a software-defined radio using a PS7 hard-core processor, DDS, and FIR filters as well software to adjust DDS peripherals and stream data via UDP to a real-time display.

To generate .bit.bin:
Execute make_project.bat from terminal

This project uses Vivado 2022.1 but can be adjusted by editing system.tcl under /tcl.

`test_fifo.c`:
- Build: `gcc -o test_fifo.c test_fifo`
- Execute: `./test_fifo`
The FIFO is located at 0x43c10000.
This will read 480000 samples and output elapsed time.

`test_udp.c`:
- Build: `gcc -o test_udp.c test_udp`
- Execute: `./test_udp <ip_address> <num_packets>`
This will send X number of packets to specified IP address.

`main.c`:
- Build: `gcc -lpthread -o main main.c -I.`
- Execute: `./main <ip_address>`
This will run the main program which acts as Linux Software-Defined Radio with UDP streaming capability.

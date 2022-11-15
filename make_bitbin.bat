: this file assumes you have a bitfile in the impl_1 directory, and turns it into a bit.bin file
: for fpgautil
call c:\Xilinx\Vivado\2022.1\settings64.bat
call bootgen -image makebin.bif -arch zynq -process_bitstream bin -w on

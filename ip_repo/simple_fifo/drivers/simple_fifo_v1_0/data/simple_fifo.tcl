

proc generate {drv_handle} {
	xdefine_include_file $drv_handle "xparameters.h" "simple_fifo" "NUM_INSTANCES" "DEVICE_ID"  "C_S_AXI_BASEADDR" "C_S_AXI_HIGHADDR"
}

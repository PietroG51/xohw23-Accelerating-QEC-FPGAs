#1. source the required sh files for Vitis and XRT:
	source path/to/settings64.sh
	source path/to/xrt/setup.sh
	export PLATFORM_PATH=path/to/xilinx/platforms/
#2. to build and run software emulation (for u55c):
	make all TARGET=sw_emu
	make run TARGET=sw_emu

#3. to build and run hardware emulation (for u55c):
	make all TARGET=hw_emu
	make run TARGET=hw_emu

#4. to build and run hardware (for u55c):
	make all TARGET=hw
	make run TARGET=hw

#5. to only compile the host (called ./querk) and run separately:
	make host
	./querk path/to/querk.xclbin

#6. to run the software version of the accelerated component:
	python3 src/software.py
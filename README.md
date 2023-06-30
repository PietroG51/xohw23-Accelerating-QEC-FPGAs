# xohw23-Accelerating-QEC-FPGAs

- **Team Number:** xohw23-251
- **Project Name:** Accelerating Quantum Error Correction through FPGAs 
- **Link to YouTube Video(s):** https://youtu.be/GexVrHjFZVw
- **Link to project repository:** https://github.com/PietroG51/xohw23-Accelerating-QEC-FPGAs
- **University name:** Politecnico di Milano

## Team:

- **Participant(s):** Valentino Guerrini,  **Email:** valentino.guerrini@mail.polimi.it

- **Participant(s):** Marco Venere,  **Email:** marco.venere@mail.polimi.it
  
- **Participant(s):** Pietro Giannoccaro,  **Email:** pietro.giannoccaro@mail.polimi.it

- **Participant(s):** Beatrice Branchini,  **Email:** beatrice.branchini@polimi.it

**Supervisor name:** Prof. Marco Santambrogio

**Supervisor e-mail:** marco.santambrogio@polimi.it

## General Info

**Board used:** Alveo U55C High Performance Compute Card

**Software Version:** 2022.2

## Brief description of project:
This project presents QuErK, a framework to accelerate Quantum Error Correction on FPGAs, achieving a top speedup of 49x with respect to software. We consider the Sparse Blossom Algorithm, that is devoted to find the best correction to apply to qubits, and implement a dataflow architecture with pipelining to accelerate the most compute-intensive component of such an algorithm.

## Description of archive:

**src folder:** contains the source files for kernel and host
-	kernel_dataflow.h : header file for the kernel
-	kernel_dataflow.cpp : source file for the kernel
-	host.cpp : source file for the host
-	host_arguments.hpp : utility source file for the host, for n = 22000 neighbors
-	software.py : source file for the software version of the accelerated component
-	software_arguments.py : utility source file for pure software, for n = 22000 neighbors
  
**Makefile:** makefile to automate design generation, build the host application and run software emulation, hardware emulation and hardware.
- makefile_us_alveo.mk : utility file for Makefile
- utils.mk : utility file for Makefile
- querk.cfg : cfg file used by Makefile
- setup.sh : script file to get most common commands.

  **bin folder:** contains the compiled hostcode and the hardware kernel for Alveo u55c
-	querk : hostcode
-	querk.xclbin : hardware kernel for Alveo u55c

  
## Instructions to build and test project
1. source the required sh files for Vitis and XRT:
	- source path/to/settings64.sh
	- source path/to/xrt/setup.sh
	- export PLATFORM_PATH=path/to/xilinx/platforms/
2. to build and run software emulation (for u55c):
	- make all TARGET=sw_emu
	- make run TARGET=sw_emu
	
3. to build and run hardware emulation (for u55c):
	- make all TARGET=hw_emu
	- make run TARGET=hw_emu
	
4. to build and run hardware (for u55c):
	- make all TARGET=hw
	- make run TARGET=hw
	
5. to only compile the host (called ./querk) and run separately:
	- make host
	- ./querk path/to/querk.xclbin
6. to run the software version of the accelerated component:
	- python3 src/software.py

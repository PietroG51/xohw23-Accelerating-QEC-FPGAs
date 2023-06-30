/******************************************
*MIT License
*
# *Copyright (c) [2023] [Valentino Guerrini, Marco Venere, Pietro Giannoccaro, Beatrice Branchini, Davide Conficconi, Marco Santambrogio]
*
*Permission is hereby granted, free of charge, to any person obtaining a copy
*of this software and associated documentation files (the "Software"), to deal
*in the Software without restriction, including without limitation the rights
*to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*copies of the Software, and to permit persons to whom the Software is
*furnished to do so, subject to the following conditions:
*
*The above copyright notice and this permission notice shall be included in all
*copies or substantial portions of the Software.
*
*THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*SOFTWARE.
******************************************/

#include <fstream>
#include "../xcl2.hpp"
#include <algorithm>
#include <iostream>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>
#include <climits>
#include <time.h>
#include <chrono>
#include <cstdint>

//PARAMETERS DEFINITION
#include "host_arguments.hpp"


#define PORT_WIDTH 32

#define NUM_KERNEL 1

#define NOW std::chrono::high_resolution_clock::now();

#define MAX_HBM_BANKCOUNT 32
#define BANK_NAME(n) n | XCL_MEM_TOPOLOGY
const int bank[MAX_HBM_BANKCOUNT] = {
    BANK_NAME(0),  BANK_NAME(1),  BANK_NAME(2),  BANK_NAME(3),  BANK_NAME(4),
    BANK_NAME(5),  BANK_NAME(6),  BANK_NAME(7),  BANK_NAME(8),  BANK_NAME(9),
    BANK_NAME(10), BANK_NAME(11), BANK_NAME(12), BANK_NAME(13), BANK_NAME(14),
    BANK_NAME(15), BANK_NAME(16), BANK_NAME(17), BANK_NAME(18), BANK_NAME(19),
    BANK_NAME(20), BANK_NAME(21), BANK_NAME(22), BANK_NAME(23), BANK_NAME(24),
    BANK_NAME(25), BANK_NAME(26), BANK_NAME(27), BANK_NAME(28), BANK_NAME(29),
    BANK_NAME(30), BANK_NAME(31)};

// MAIN OF THE HOST
int main(int argc, char *argv[]){
    
    std::string binaryFile = "querk.xclbin";
	std::string readsPath;
	
    cl::Context context;
 
    cl::CommandQueue commands;
	
    if (argc == 2) { //Input provided by file 
         binaryFile = argv[1];
    }   
    
    //INITIALIZATION

    std::string krnl_name = "querk";
    cl::Kernel krnl;

    // The get_xil_devices will return vector of Xilinx Devices
    auto devices = xcl::get_xil_devices();

    // read_binary_file() command will find the OpenCL binary file created using the
    // V++ compiler load into OpenCL Binary and return pointer to file buffer.
    auto fileBuf = xcl::read_binary_file(binaryFile);

    cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
    int valid_device = 0;

    cl_int err;

    for (unsigned int i = 0; i < devices.size(); i++) {
        auto device = devices[i];
            // Creating Context and Command Queue for selected Device
        OCL_CHECK(err, context = cl::Context(device, NULL, NULL, NULL, &err));
        OCL_CHECK(err, commands = cl::CommandQueue(context, device,
                            CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE | CL_QUEUE_PROFILING_ENABLE, &err));

        std::cout << "Trying to program device[" << i 
                  << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
     
        cl::Program program(context, {device}, bins, NULL, &err);
        
        if (err != CL_SUCCESS) {
            std::cout << "Failed to program device[" << i
                        << "] with xclbin file!\n";                      
        } else {
            std::cout << "Device[" << i << "]: program successful!\n";
            
            // Creating Kernel object using Compute unit names
            
            std::string cu_id = std::to_string(1);
            std::string krnl_name_full = krnl_name + ":{" + "querk_" + cu_id + "}";

            printf("Creating a kernel [%s] for CU(%d)\n", krnl_name_full.c_str(),  1);

            //Here Kernel object is created by specifying kernel name along with compute unit.
            //For such case, this kernel object can only access the specific Compute unit
            OCL_CHECK(err, krnl = cl::Kernel(program, krnl_name_full.c_str(), &err));
            

            valid_device++;
            break; // we break because we found a valid device
        }
        std::cout<<"dwvgae"<<std::endl;
    }

	std::cout<<"Kernel created"<<std::endl;
    
    if (valid_device == 0) {
        std::cout << "Failed to program any device found, exit!\n";
        exit(EXIT_FAILURE);
    }

    // CREATE DEVICE BUFFERS
    cl_mem_ext_ptr_t num_neighbors_ext;
    cl_mem_ext_ptr_t radius_ext;
    cl_mem_ext_ptr_t region_that_arrived_top_ext;
    cl_mem_ext_ptr_t wrapped_radius_cached_ext;
    cl_mem_ext_ptr_t neighbors_ext;
    cl_mem_ext_ptr_t neighbor_weights_ext;
    cl_mem_ext_ptr_t neighbor_observables_ext;
    cl_mem_ext_ptr_t out_neighbor_ext;
    cl_mem_ext_ptr_t out_time_ext;
    cl::Buffer num_neighbors_buffer;
    cl::Buffer radius_buffer;
    cl::Buffer region_that_arrived_top_buffer;
    cl::Buffer wrapped_radius_cached_buffer;
    cl::Buffer neighbors_buffer;
    cl::Buffer neighbor_weights_buffer;
    cl::Buffer neighbor_observables_buffer;
    cl::Buffer out_neighbor_buffer;
    cl::Buffer out_time_buffer;


    num_neighbors_ext.obj = num_neighbors.data();
    num_neighbors_ext.param = 0;
    num_neighbors_ext.flags = bank[0];

    radius_ext.obj = radius.data();
    radius_ext.param = 0;
    radius_ext.flags = bank[1];
    
    region_that_arrived_top_ext.obj = region_that_arrived_top.data();
    region_that_arrived_top_ext.param = 0;
    region_that_arrived_top_ext.flags = bank[2];

    wrapped_radius_cached_ext.obj = wrapped_radius_cached.data();
    wrapped_radius_cached_ext.param = 0;
    wrapped_radius_cached_ext.flags = bank[3];

    neighbors_ext.obj = neighbors.data();
    neighbors_ext.param = 0;
    neighbors_ext.flags = bank[4];

    neighbor_weights_ext.obj = neighbor_weights.data();
    neighbor_weights_ext.param = 0;
    neighbor_weights_ext.flags = bank[5];
    
    neighbor_observables_ext.obj = neighbor_observables.data();
    neighbor_observables_ext.param = 0;
    neighbor_observables_ext.flags = bank[6];

    out_neighbor_ext.obj = out_neighbor.data();
    out_neighbor_ext.param = 0;
    out_neighbor_ext.flags = bank[7];

    out_time_ext.obj = out_time.data();
    out_time_ext.param = 0;
    out_time_ext.flags = bank[8];



    OCL_CHECK(err, num_neighbors_buffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX |
                                CL_MEM_USE_HOST_PTR, sizeof(int)*NUM_NODES, &num_neighbors_ext, &err));
    OCL_CHECK(err, radius_buffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX |
                                CL_MEM_USE_HOST_PTR, sizeof(long int)*NUM_REGIONS, &radius_ext, &err));
    OCL_CHECK(err, region_that_arrived_top_buffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX |
                                CL_MEM_USE_HOST_PTR, sizeof(int)*NUM_NODES, &region_that_arrived_top_ext, &err));
    OCL_CHECK(err, wrapped_radius_cached_buffer = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX |
                                CL_MEM_USE_HOST_PTR, sizeof(int)*NUM_NODES, &wrapped_radius_cached_ext, &err));
    OCL_CHECK(err, neighbors_buffer = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX |
                                CL_MEM_USE_HOST_PTR, sizeof(int)*NUM_NODES*NUM_NEIGHBORS, &neighbors_ext, &err));
	
    OCL_CHECK(err, neighbor_weights_buffer = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX |
                                CL_MEM_USE_HOST_PTR, sizeof(int)*NUM_NODES*NUM_NEIGHBORS, &neighbor_weights_ext, &err));

    OCL_CHECK(err, neighbor_observables_buffer = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX |
                                CL_MEM_USE_HOST_PTR, sizeof(long int)*NUM_NODES*NUM_NEIGHBORS, &neighbor_observables_ext, &err));

    OCL_CHECK(err, out_neighbor_buffer = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX |
                                CL_MEM_USE_HOST_PTR, sizeof(int), &out_neighbor_ext, &err));
    
    OCL_CHECK(err, out_time_buffer = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX |
                              CL_MEM_USE_HOST_PTR, sizeof(long int), &out_time_ext, &err));



	commands.finish();

    // WRITE OUR DATA INTO THE DEVICE BUFFER  
     
    err = commands.enqueueMigrateMemObjects({num_neighbors_buffer, radius_buffer, region_that_arrived_top_buffer, wrapped_radius_cached_buffer, neighbors_buffer, neighbor_weights_buffer, neighbor_observables_buffer, out_neighbor_buffer, out_time_buffer}, 0);

    if (err != CL_SUCCESS) {
            printf("Error: Failed to write to device memory!\n");
            printf("Test failed\n");
            exit(1);
    }

	commands.finish();
    
    // SET THE ARGUMENTS TO THE KERNEL

    OCL_CHECK(err, err = krnl.setArg(0, detector_node));
    OCL_CHECK(err, err = krnl.setArg(1, num_nodes));
    OCL_CHECK(err, err = krnl.setArg(2, num_regions));
    OCL_CHECK(err, err = krnl.setArg(3, num_neighbors_buffer));
    OCL_CHECK(err, err = krnl.setArg(4, radius_buffer));
    OCL_CHECK(err, err = krnl.setArg(5, region_that_arrived_top_buffer));
    OCL_CHECK(err, err = krnl.setArg(6, wrapped_radius_cached_buffer));
    OCL_CHECK(err, err = krnl.setArg(7, neighbors_buffer));
    OCL_CHECK(err, err = krnl.setArg(8, neighbor_weights_buffer));
    OCL_CHECK(err, err = krnl.setArg(9, neighbor_observables_buffer));
    OCL_CHECK(err, err = krnl.setArg(10, out_neighbor_buffer));
    OCL_CHECK(err, err = krnl.setArg(11, out_time_buffer));

    if (err != CL_SUCCESS) {
        printf("Error: Failed to set kernel arguments! %d\n", err);
        printf("Test failed\n");
        exit(1);
    }
    

	commands.finish();

    std::chrono::high_resolution_clock::time_point start = NOW;
    
    // EXECUTE THE KERNEL
    
    err = commands.enqueueTask(krnl);


 

    commands.finish();
    
    
    std::chrono::high_resolution_clock::time_point end1 = NOW;
    if (err) {
    printf("Error: Failed to execute kernel! %d\n", err);
    printf("Test failed\n");
    exit(1);
}
	std::chrono::duration<double> time = std::chrono::duration_cast<std::chrono::duration<double>>(end1-start);
    
    // READ BACK THE RESULTS FROM THE DEVICE
  
    err = commands.enqueueMigrateMemObjects({out_neighbor_buffer, out_time_buffer}, CL_MIGRATE_MEM_OBJECT_HOST);  
    commands.finish();
    
    //uncomment to print output. Please notice: output fields are structured as bitmasks for proper usage by the algorithm
    //printf("Hardware results: %u %lu\n", (int) out_neighbor[0], (long int) out_time[0] );

    if (err != CL_SUCCESS) {
        printf("Error: Failed to read output array! %d\n", err);
        printf("Test failed\n");
        exit(1);
    }

	printf("HW time: %lf\n", time.count());

}

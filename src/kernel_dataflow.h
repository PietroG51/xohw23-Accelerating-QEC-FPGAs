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

#ifndef KERNEL_DATAFLOW_H
#define KERNEL_DATAFLOW_H

//PARAMETERS DEFINITION

#define NUM_NODES 100
#define NUM_REGIONS 10
#define NUM_NEIGHBORS 2
#define MAX 9223372036854775807

//TOP FUNCTION OF THE KERNEL

extern "C" void querk(ap_uint<32> detector_node, ap_uint<32> num_nodes, ap_uint<32> num_regions, ap_uint<32> * num_neighbors, ap_uint<64> * radius, ap_uint<32> * region_that_arrived_top, ap_uint<32> * wrapped_radius_cached, ap_uint<32> neighbors[][NUM_NEIGHBORS], ap_uint<32> neighbor_weights[][NUM_NEIGHBORS], ap_uint<64> neighbor_observables[][NUM_NEIGHBORS], ap_uint<32> * out_neighbor, ap_uint<64> * out_time);
#endif

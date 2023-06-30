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

#include <hls_stream.h>

#include <cstdlib>
#include <cstring>
#include "ap_int.h"
#include "kernel_dataflow.h"

//PARAMETERS DEFINITION

const int fifo_in_depth = 100000;

#define NUM_NODES 100000
#define NUM_REGIONS 100000
#define NUM_NEIGHBORS 2
#define MAX 9223372036854775807

//COMPONENTS FOR THE TASKS OF THE DATAFLOW ARCHITECTURE

//This component initializes the data in the HLS Stream for the dataflow architecture.
void init_data(
    hls::stream<ap_uint<64> >& rad1,
    hls::stream<ap_uint<64> >& rad1_2,
    hls::stream<ap_uint<32> >& rtat,
    hls::stream<ap_uint<32> >& nn,
    hls::stream<ap_uint<32> >& nn_2,
    hls::stream<ap_uint<32> >& best_neighbor,
    hls::stream<ap_uint<64> >& collision_time,
    hls::stream<ap_uint<64> >& best_time,
    hls::stream<ap_uint<32> >& start_1,
    hls::stream<ap_uint<32> >& start_2,
    hls::stream<ap_uint<32> >& neighbor_weights_stream,
    hls::stream<ap_uint<32> >& wrapped_radius_cached_stream,
    hls::stream<ap_uint<32> >& region_that_arrived_top_stream,
    hls::stream<ap_uint<64> >& radius_stream,
    ap_uint<32> * region_that_arrived_top,
    ap_uint<32> * num_neighbors,
    ap_uint<32> * wrapped_radius_cached,
    ap_uint<64> * radius,
    ap_uint<32> neighbors[][NUM_NEIGHBORS],
    ap_uint<32> neighbor_weights[][NUM_NEIGHBORS],
    ap_uint<32> detector_node



    ){


        ap_uint<32> rtat_tmp = region_that_arrived_top[detector_node];
        rtat << rtat_tmp;
        ap_uint<32> nn_tmp = num_neighbors[detector_node];
        nn << nn_tmp;
        nn_2 << nn_tmp;
        ap_uint<64> rad1_tmp;
        ap_uint<32> start_tmp = 0;

        ap_uint<32> best_neighbor_tmp = MAX;
        ap_uint<64> best_time_tmp = MAX;
        ap_uint<64> collision_time_tmp;


        if(rtat_tmp == -1){
            rad1_tmp =0;
            rad1 << rad1_tmp;
            rad1_2 << rad1_tmp;
        }else{
            rad1_tmp = radius[rtat_tmp] + (wrapped_radius_cached[detector_node] << 2);
            rad1 << rad1_tmp;
            rad1_2 << rad1_tmp;
        }

        if(!(nn_tmp==0) && neighbors[detector_node][0] == -1){
            start_tmp=1;
            
        }

        start_1 << start_tmp;
        start_2 << start_tmp;

        if((rad1_tmp &1) && !(nn_tmp==0) && neighbors[detector_node][0] == -1){
            ap_uint<32> weight = neighbor_weights[detector_node][0];
            collision_time_tmp = weight - ( (rad1_tmp >> 2) << 2);

            if(collision_time_tmp < best_time_tmp){
                best_time_tmp = collision_time_tmp;
                best_neighbor_tmp = 0;
            }

        }
        best_time << best_time_tmp;
        best_neighbor << best_neighbor_tmp;
        collision_time << collision_time_tmp;

        for(int i=start_tmp;i<nn_tmp;i++){

        #pragma HLS LOOP_TRIPCOUNT min =0 max = fifo_in_depth
            neighbor_weights_stream << neighbor_weights[detector_node][i];
            ap_uint<32> tmp=neighbors[detector_node][i];

            region_that_arrived_top_stream << region_that_arrived_top[tmp];

            wrapped_radius_cached_stream << wrapped_radius_cached[tmp];

            radius_stream << radius[region_that_arrived_top[tmp]];

        }

    }


//This component computes the radius of the region and the condition for the validity of the update of the collision time
void compute_radius_and_valid(hls::stream<ap_uint<32> >& start_1,
            hls::stream<bool> &valid_stream,
            hls::stream<ap_uint<64> > &rad2_stream,
            hls::stream<ap_uint<32> >& region_that_arrived_top_stream,
            hls::stream<ap_uint<32> > &wrapped_radius_cached_stream,
            hls::stream<ap_uint<64> > &radius_stream,
            hls::stream<ap_uint<64> >& rad1,
            hls::stream<ap_uint<32> >& rtat,
            hls::stream<ap_uint<32> >& nn){
    ap_uint<32> start_tmp = start_1.read();
    ap_uint<64> rad1_tmp = rad1.read();
    ap_uint<32> rtat_tmp = rtat.read();
    ap_uint<32> nn_tmp = nn.read();



    for(int i=start_tmp;i<nn_tmp;i++){
        #pragma HLS LOOP_TRIPCOUNT min =0 max = fifo_in_depth
        ap_uint<32> rtatn = region_that_arrived_top_stream.read();

        if((rad1_tmp & 1) && rtat_tmp==rtatn){
            valid_stream << false;            
        }else{
            valid_stream << true;
        }

        if(rtatn == -1){
            rad2_stream << 0;
        }else{
            rad2_stream << (radius_stream.read() + (wrapped_radius_cached_stream.read()<<2));
        }


    }
}

//This component computes the collision time
void compute_collision(hls::stream<ap_uint<32> >& start_2,
            hls::stream<ap_uint<32> >& best_neighbor,
            hls::stream<ap_uint<64> >& best_time,
            hls::stream<ap_uint<64> >& collision_time, 
            hls::stream<ap_uint<64> >& rad_2_stream,
            hls::stream<ap_uint<32> > &neighbor_weights_stream,
            hls::stream<bool>& valid_stream,
            hls::stream<ap_uint<64> >& rad1,
            hls::stream<ap_uint<32> >& nn,
            ap_uint<32> * out_neighbor, ap_uint<64> * out_time){

    ap_uint<32> start_tmp = start_2.read();
    ap_uint<64> rad1_tmp = rad1.read();
    ap_uint<32> nn_tmp = nn.read();

    ap_uint<32> best_neighbor_tmp = best_neighbor.read();
    ap_uint<64> best_time_tmp = best_time.read();
    ap_uint<64> collision_time_tmp = collision_time.read();



    for(int i=start_tmp;i<nn_tmp;i++){
        #pragma HLS LOOP_TRIPCOUNT min =0 max = fifo_in_depth
        

        ap_uint<64>  rad2 = rad_2_stream.read();
        bool valid = valid_stream.read();

        ap_uint<32> weight = neighbor_weights_stream.read();
        if(valid && !((rad1_tmp & 1) && (rad2 & 2))){

            if((rad1_tmp & 1)) {
	            collision_time_tmp = weight - ((rad1_tmp >> 2) << 2) - ((rad2 >> 2) << 2);
	        }
	        if ((rad1_tmp & 1) && (rad2 & 1)) {
	            collision_time_tmp >>= 1;
	        }
            if ((rad1_tmp & 1) && collision_time_tmp < best_time_tmp) {
                    best_time_tmp = collision_time_tmp;
                    best_neighbor_tmp = i;
            }
                if (!(rad1_tmp & 1) && (rad2 & 1)) {
                	//auto
                    ap_uint<64> collision_time_tmp = weight - ((rad1_tmp >> 2) << 2) - ((rad2 >> 2) << 2);
                    if (collision_time_tmp < best_time_tmp) {
                        best_time_tmp = collision_time_tmp;
                        best_neighbor_tmp = i;
                    }
                }

        }
 
    }
    *out_neighbor= best_neighbor_tmp;
    *out_time = best_time_tmp;

    

}

//TOP FUNCTION OF THE KERNEL

extern "C" void querk(ap_uint<32> detector_node, ap_uint<32> num_nodes, ap_uint<32> num_regions, ap_uint<32> * num_neighbors, ap_uint<64> * radius, ap_uint<32> * region_that_arrived_top, ap_uint<32> * wrapped_radius_cached, ap_uint<32> neighbors[][NUM_NEIGHBORS], ap_uint<32> neighbor_weights[][NUM_NEIGHBORS], ap_uint<64> neighbor_observables[][NUM_NEIGHBORS], ap_uint<32> * out_neighbor, ap_uint<64> * out_time) {

#pragma HLS INTERFACE m_axi port=region_that_arrived_top depth=fifo_in_depth offset=slave bundle=gmem0
#pragma HLS INTERFACE m_axi port=out_neighbor depth=1 offset=slave bundle=gmem1
#pragma HLS INTERFACE m_axi port=num_neighbors depth=fifo_in_depth offset=slave bundle=gmem2
#pragma HLS INTERFACE m_axi port=out_time depth=1 offset=slave bundle=gmem3
#pragma HLS INTERFACE m_axi port=wrapped_radius_cached depth=fifo_in_depth offset=slave bundle=gmem4
#pragma HLS INTERFACE m_axi port=radius depth=fifo_in_depth offset=slave bundle=gmem5
#pragma HLS INTERFACE m_axi port=neighbors depth=fifo_in_depth offset=slave bundle=gmem6
#pragma HLS INTERFACE m_axi port=neighbor_weights depth=fifo_in_depth offset=slave bundle=gmem7
#pragma HLS INTERFACE m_axi port=neighbor_observables depth=fifo_in_depth offset=slave bundle=gmem8

#pragma HLS INTERFACE s_axilite port=detector_node bundle=control
#pragma HLS INTERFACE s_axilite port=num_nodes bundle=control
#pragma HLS INTERFACE s_axilite port=num_regions bundle=control
#pragma HLS INTERFACE s_axilite port=num_neighbors bundle=control
#pragma HLS INTERFACE s_axilite port=region_that_arrived_top bundle=control
//#pragma HLS INTERFACE s_axilite port=m  bundle=control
#pragma HLS INTERFACE s_axilite port=wrapped_radius_cached bundle=control
#pragma HLS INTERFACE s_axilite port=radius bundle=control
#pragma HLS INTERFACE s_axilite port=neighbors bundle=control
#pragma HLS INTERFACE s_axilite port=neighbor_weights bundle=control
#pragma HLS INTERFACE s_axilite port=neighbor_observables bundle=control
#pragma HLS INTERFACE s_axilite port=out_neighbor bundle=control
#pragma HLS INTERFACE s_axilite port=out_time bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

static hls::stream<ap_uint<64> > rad1("rad1_stream");
static hls::stream<ap_uint<64> > rad1_2("rad1_2_stream");

static hls::stream<ap_uint<32> > rtat("rtat_stream");

static hls::stream<ap_uint<32> > nn("nn_stream");
static hls::stream<ap_uint<32> > nn_2("nn_2_stream");

static hls::stream<ap_uint<32> > best_neighbor("best_neighbor_stream");
static hls::stream<ap_uint<64> > collision_time("collision_time_stream");
static hls::stream<ap_uint<64> > best_time("best_time_stream");

static hls::stream<ap_uint<32> > start_1("start_stream");
static hls::stream<ap_uint<32> > start_2("start_2_stream");


static hls::stream<ap_uint<32> > neighbor_weights_stream("neighbor_weights_stream");
static hls::stream<ap_uint<32> > region_that_arrived_top_stream("region_that_arrived_top_stream");
static hls::stream<ap_uint<32> > wrapped_radius_cached_stream("wrapped_radius_cached_stream");
static hls::stream<ap_uint<64> > radius_stream("radius_stream");
static hls::stream<bool> valid_stream("valid_stream");
static hls::stream<ap_uint<64> > rad_2_stream("rad_2_stream");

#pragma HLS STREAM variable = rad1 depth = 2
#pragma HLS STREAM variable = rad1_2 depth = 3

#pragma HLS STREAM variable = rtat depth = 2
#pragma HLS STREAM variable = nn depth = 2
#pragma HLS STREAM variable = nn_2 depth = 3

#pragma HLS STREAM variable = best_neighbor depth = 3
#pragma HLS STREAM variable = collision_time depth = 3
#pragma HLS STREAM variable = best_time depth = 3
#pragma HLS STREAM variable = start_1 depth = 2
#pragma HLS STREAM variable = start_2 depth = 3


#pragma HLS STREAM variable = neighbor_weights_stream depth = fifo_in_depth
#pragma HLS STREAM variable = region_that_arrived_top_stream depth = fifo_in_depth
#pragma HLS STREAM variable = wrapped_radius_cached_stream depth = fifo_in_depth
#pragma HLS STREAM variable = radius_stream depth = fifo_in_depth
#pragma HLS STREAM variable = valid_stream depth = fifo_in_depth
#pragma HLS STREAM variable = rad_2_stream depth = fifo_in_depth

//DATAFLOW ARCHITECTURE

#pragma HLS dataflow


init_data(rad1,
            rad1_2,
            rtat,
            nn,
            nn_2,
            best_neighbor,
            collision_time,
            best_time,
            start_1,
            start_2,
            neighbor_weights_stream,
            wrapped_radius_cached_stream,
            region_that_arrived_top_stream,
            radius_stream,
            region_that_arrived_top,
            num_neighbors,
            wrapped_radius_cached,
            radius,
            neighbors,
            neighbor_weights,
            detector_node);

compute_radius_and_valid(start_1,valid_stream,rad_2_stream,region_that_arrived_top_stream,wrapped_radius_cached_stream,radius_stream,rad1,rtat,nn);

compute_collision(start_2,best_neighbor,best_time,collision_time,rad_2_stream,neighbor_weights_stream,valid_stream,rad1_2,nn_2,out_neighbor,out_time);












}

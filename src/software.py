"""
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
"""

import numpy as np
from software_arguments import *
import time

#SOFTWARE VERSION OF THE ACCELERATED COMPONENT

def find_next_event_at_node_occupied_by_growing_top_region(detector_node, rad1, num_neighbors, neighbors, neighbor_weights, region_that_arrived_Top, wrapped_radius_cached, radius):
    best_time = np.infty
    best_neighbor = np.infty
    start = 0
    if (not (num_neighbors[detector_node]==0) and neighbors[detector_node][0] == -1):
        # Growing towards boundary
        weight = neighbor_weights[detector_node][0]
        collision_time = weight - ((rad1 >> 2)<<2)
        if (collision_time < best_time):
            best_time = collision_time
            best_neighbor = 0
        
        start+= 1
    
    #Handle non-boundary neighbors.
    for i in range(start, num_neighbors[detector_node]):
        
        weight = neighbor_weights[detector_node][i]

        neighbor = neighbors[detector_node][i]

        if (region_that_arrived_top[detector_node] == region_that_arrived_top[neighbor]):
            continue


        if (region_that_arrived_top[neighbor] == -1):
                rad2 = 0
        else:
            rad2 = radius[region_that_arrived_top[neighbor]] +(wrapped_radius_cached[neighbor] << 2)


        if (rad2 & 2):
            continue

        collision_time = weight - ((rad1 >> 2) << 2) - ((rad2 >> 2) << 2)
        if (rad2 & 1):
            collision_time >>= 1
        if (collision_time < best_time):
            best_time = collision_time
            best_neighbor = i
    return best_neighbor, best_time


def find_next_event_at_node_not_occupied_by_growing_top_region(detector_node,
	rad1,
	num_neighbors,
	neighbors,
	neighbor_weights,
	region_that_arrived_top,
	wrapped_radius_cached,
	radius):
        
    best_time = np.infty
    best_neighbor = np.infty

    start = 0
    if (not(num_neighbors[detector_node]==0) and neighbors[detector_node][0] == -1):
        start+=1

    # Handle non-boundary neighbors.
    for i in range(start, num_neighbors[detector_node]):

        weight = neighbor_weights[detector_node][i]

        neighbor = neighbors[detector_node][i]


        if (region_that_arrived_top[neighbor] == -1):
            rad2 = 0
        else:
            # print(neighbor)
            # print(region_that_arrived_top[neighbor])
            rad2 = radius[region_that_arrived_top[neighbor]] +(wrapped_radius_cached[neighbor] << 2)


        if (rad2 & 1):
            collision_time = weight - ((rad1 >> 2) << 2) - ((rad2 >> 2) << 2)
            if (collision_time < best_time):
                best_time = collision_time
                best_neighbor = i
        

    
    return best_neighbor, best_time


def find_next_event_at_node_returning_neighbor_index_and_time(
    detector_node,
	num_neighbors,
	neighbors,
	neighbor_weights,
	region_that_arrived_top,
	wrapped_radius_cached,
	radius):

    if (region_that_arrived_top[detector_node] == -1):
            rad1 = 0
    else:
        rad1 = radius[region_that_arrived_top[detector_node]] + (wrapped_radius_cached[detector_node] << 2)



    if (rad1 & 1):
        return find_next_event_at_node_occupied_by_growing_top_region(detector_node, rad1, num_neighbors, neighbors, neighbor_weights, region_that_arrived_top, wrapped_radius_cached, radius)
    else:
        return find_next_event_at_node_not_occupied_by_growing_top_region(detector_node, rad1, num_neighbors, neighbors, neighbor_weights, region_that_arrived_top, wrapped_radius_cached, radius)


#COMPONENT EXECUTION

start = time.time()
output = find_next_event_at_node_returning_neighbor_index_and_time(
    detector_node,
	num_neighbors,
	neighbors,
	neighbor_weights,
	region_that_arrived_top,
	wrapped_radius_cached,
	radius)
end = time.time()
import ctypes
#Notice: output of the algorithm is a bitmask that will be managed by the algorithm, with specific bit flags set.
print("Overall time: ", end-start)
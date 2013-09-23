#!/bin/bash

patch -o ./lulesh_main_only.cc lulesh.cc patch_main_only.txt
patch -o ./lulesh_core_functions.cc lulesh.cc patch_core_functions.txt
patch -o ./lulesh_loop_only.cc lulesh.cc patch_loop_only.txt
patch -o ./lulesh_iterations_based.cc lulesh.cc patch_iterations_based.txt

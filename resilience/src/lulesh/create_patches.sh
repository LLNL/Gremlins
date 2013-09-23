#!/bin/bash

diff -Naur lulesh.cc lulesh_main_only.cc > patch_main_only.txt
diff -Naur lulesh.cc lulesh_core_functions.cc > patch_core_functions.txt
diff -Naur lulesh.cc lulesh_loop_only.cc > patch_loop_only.txt
diff -Naur lulesh.cc lulesh_iterations_based.cc > patch_iterations_based.txt

##############################################################################
# Copyright (c) 2013-2014, Lawrence Livermore National Security, LLC.
# Produced at the Lawrence Livermore National Laboratory
#
# Written by Martin Schulz et al <schulzm@llnl.gov>
# LLNL-CODE-645436
# All rights reserved.
#
# This file is part of the GREMLINS framework.
# For details, see http://scalability.llnl.gov/gremlins.
# Please read the LICENSE file for further information.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the disclaimer below.
#
#     * Redistributions in binary form must reproduce the above
#       copyright notice, this list of conditions and the disclaimer (as
#       noted below) in the documentation and/or other materials
#       provided with the distribution.
#
#     * Neither the name of the LLNS/LLNL nor the names of its
#       contributors may be used to endorse or promote products derived
#       from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL LAWRENCE
# LIVERMORE NATIONAL SECURITY, LLC, THE U.S. DEPARTMENT OF ENERGY OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
##############################################################################

# Author: Ignacio Laguna
# Contact: ilaguna@llnl.gov


# C++ compiler
CXX             = mpicc
CC              = $(CXX)

# Passed to compiler
OPT_LEVEL       = -O2
DEBUG_LEVEL     = -g
EXTRA_CCFLAGS   = -Wall
CXXFLAGS        = $(DEBUG_LEVEL) $(EXTRA_CCFLAGS) $(OPT_LEVEL) -fPIC
CCFLAGS         = $(CXXFLAGS)

# Passed to the C pre-processor
CPPFLAGS        = -I./

# Passed to the linker
LDFLAGS         = -L./

# Link with these libraries
LDLIBS          =

# Installation path


#CPPFLAGS	= -I$(CURDIR)/../fault_injector 
#LDFLAGS		= -L$(CURDIR)/../fault_injector -l_gremlin_instruction_faults 

CODE		= lulesh_main_only lulesh_loop_only lulesh_core_functions lulesh_iterations_based

SRC_FILES	= lulesh_main_only.cc lulesh_loop_only.cc lulesh_core_functions.cc lulesh_iterations_based.cc

O_FILES		= $(SRC_FILES:%.cc=%.o)

all: $(CODE)

$(CODE): $(O_FILES)
	$(CXX)  $(LDFLAGS) -o lulesh_main_only lulesh_main_only.o
	$(CXX)  $(LDFLAGS) -o lulesh_loop_only lulesh_loop_only.o
	$(CXX)  $(LDFLAGS) -o lulesh_core_functions lulesh_core_functions.o
	$(CXX)  $(LDFLAGS) -o lulesh_iterations_based lulesh_iterations_based.o

clean:
	$(RM) *.o $(CODE)

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

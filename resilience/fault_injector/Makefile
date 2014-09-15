# C++ compiler
CXX 		= g++
CC		= $(CXX)

# Passed to compiler
OPT_LEVEL	= -O2
DEBUG_LEVEL	= -g
EXTRA_CCFLAGS	= -Wall
CXXFLAGS	= $(DEBUG_LEVEL) $(EXTRA_CCFLAGS) $(OPT_LEVEL) -fPIC
CCFLAGS		= $(CXXFLAGS)

# Passed to the C pre-processor
CPPFLAGS        = -I./

# Passed to the linker
LDFLAGS         = -L./

# Link with these libraries
LDLIBS          =



CODE		= lib_gremlin_instruction_faults.so
SRC_FILES	= fault_injector.cc fault_injector.h

O_FILES		= $(SRC_FILES:%.cc=%.o)

all: $(CODE)

$(CODE): $(O_FILES)
	$(CXX) -pthread -shared -o $(CODE) $(O_FILES)

clean:
	$(RM) *.o $(CODE)



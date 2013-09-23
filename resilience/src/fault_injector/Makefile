include ../../Makefile.config

CODE		= lib_gremlin_instruction_faults.so
SRC_FILES	= fault_injector.cc fault_injector.h

O_FILES		= $(SRC_FILES:%.cc=%.o)

all: $(CODE)

$(CODE): $(O_FILES)
	$(CXX) -pthread -shared -o $(CODE) $(O_FILES)

clean:
	$(RM) *.o $(CODE)

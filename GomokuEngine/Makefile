# Define the compiler
CXX = g++

# Compiler flags, e.g. -g for debugging, -O2 for optimisation and -Wall for turning on most warnings
CXXFLAGS = -g -O0 -Wall

# Define the C++ source files
SRCS = AI1.cpp gomoku_engine.cpp main.cpp

# Define the C++ object files 
OBJS = $(SRCS:.cpp=.o)

# Define the executable file 
MAIN = main

all: $(MAIN)
	echo "Successfully compiled $(MAIN) executable."

$(MAIN): $(OBJS) 
	$(CXX) $(CXXFLAGS) -o $(MAIN) $(OBJS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<  -o $@

clean:
	$(RM) *.o *~ $(MAIN)

re: clean all

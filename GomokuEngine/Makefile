# Define the compiler
CXX = g++

SRCDIR = src
INCDIR = inc
OBJDIR = obj

# Default mode is perf
MODE ?= perf

# Compiler flags, e.g. -g for debugging, -O2 for optimisation and -Wall for turning on most warnings
CXXFLAGSPERF = -O3 -Wall -I$(INCDIR)
CXXFLAGSDEBUG = -pg -Wall -I$(INCDIR)

# Conditional assignment based on the MODE variable
ifeq ($(MODE),debug)
CXXFLAGS = $(CXXFLAGSDEBUG)
else
CXXFLAGS = $(CXXFLAGSPERF)
endif

# Define the C++ source files
SRCFILES = AI1.cpp gomoku_engine.cpp gomoku_structure_pattern.cpp main.cpp
SRCS = $(addprefix $(SRCDIR)/, $(SRCFILES))

# Define the C++ object files 
OBJS = $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# Define the executable file 
MAIN = main

all: $(MAIN)
	echo "Successfully compiled $(MAIN) executable in $(MODE) mode."

$(MAIN): $(OBJS) 
	$(CXX) $(CXXFLAGS) -o $(MAIN) $(OBJS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS) $(MAIN)

re: clean all
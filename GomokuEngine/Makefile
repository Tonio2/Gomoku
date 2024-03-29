-include .env
export

# Compiler settings
CXX = g++
CXXFLAGS = -Wall -I$(INCDIR) -DDEPTH=$(DEPTH) -std=c++17

# Directories
SRCDIR = src
INCDIR = inc
OBJDIR = obj

# Mode configuration: Default is performance (PERF)
MODE ?= PERF
ifeq ($(MODE),DEBUG)
	CXXFLAGS += -pg -g # Debug flags
else
	CXXFLAGS += -O3 -DNDEBUG # Optimize for performance
endif

# Timer configuration: Disabled by default
TIMER ?= OFF
ifeq ($(TIMER),OFF)
	CXXFLAGS += -DNOTIMER # Define NOTIMER to disable timing functionality
endif

# Source and object files
SRCFILES = Timer.cpp gomoku_ai.cpp gomoku_ai_data.cpp gomoku_engine.cpp gomoku_pattern_reconizer.cpp main.cpp utils.cpp test_engine.cpp arena.cpp
SRCS = $(addprefix $(SRCDIR)/, $(SRCFILES))
OBJS = $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# Define the executable file 
MAIN = main

all: $(MAIN)
	echo "Successfully compiled $(MAIN) executable in $(MODE) mode with timer $(TIMER)."

$(MAIN): $(OBJDIR) $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(MAIN) $(OBJS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	$(RM) $(OBJS) $(MAIN)

re: clean all
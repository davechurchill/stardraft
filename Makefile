# to build the project:
#   $ cd stardraft
#   $ make
# executable binaries should be put in stardraft/bin
# to compile from the bin directory (more convenient):
#   $ cd stardraft/bin
#   $ make -C ..
# to compile with N processors, use 'make -j N'
#
# overall best work flow is to compile from bin with 8 threads:
#   $ cd stardraft/bin
#   $ make -C .. -j 8
#   $ ./stardraft
CXX ?= g++

# C++ compilation flags, be sure it's set to C++17 and optimized
stardraft_CXXFLAGS=-O3 -std=c++17

# linker flags, will link the SFML library
stardraft_LDFLAGS=-lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

# include directory(s), will include the engine files
stardraft_CPPFLAGS=-I./src

# the source files for the ecs game engine
SRC_STARDRAFT=$(wildcard src/*.cpp) 
OBJ_STARDRAFT=$(SRC_STARDRAFT:.cpp=.o)

# all of these targets will be made if you just type make
.PHONY: all
all:bin/stardraft

# define the main executable requirements / command
bin/stardraft:$(OBJ_STARDRAFT) Makefile
	$(CXX) $(stardraft_CXXFLAGS) $(CXXFLAGS) $(OBJ_STARDRAFT) -o $@ $(stardraft_LDFLAGS) $(LDFLAGS)

.cpp.o:
	$(CXX) -c $(stardraft_CXXFLAGS) $(CXXFLAGS) $(stardraft_CPPFLAGS) $(CPPFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -f $(OBJ_STARDRAFT) ./bin/stardraft

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
CC=g++

# C++ compilation flags, be sure it's set to C++17 and optimized
CFLAGS=-O3 -std=c++17

# linker flags, will link the SFML library
LDFLAGS=-O3 -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

# include directory(s), will include the engine files
INCLUDES=-I./src

# the source files for the ecs game engine
SRC_STARDRAFT=$(wildcard src/*.cpp) 
OBJ_STARDRAFT=$(SRC_STARDRAFT:.cpp=.o)

# all of these targets will be made if you just type make
all:stardraft

# define the main executable requirements / command
stardraft:$(OBJ_STARDRAFT) Makefile
	$(CC) $(OBJ_STARDRAFT) -o ./bin/$@ $(LDFLAGS)

.cpp.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@

clean:
	rm -f $(OBJ_STARDRAFT) ./bin/stardraft

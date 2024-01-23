# Makefile for MPI program

# Source file
SRC = prefix.cpp

# number of process
NP = 4

# Compiler
CC = mpic++

# Executable name
EXE = ./out

all: $(EXE) 

$(EXE): $(SRC)
	$(CC) -o $(EXE) $(SRC)

clean:
	rm -f $(EXE)

run:
	mpirun -np $(NP) -f mpi_hosts $(EXE) 
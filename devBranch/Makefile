SHELL := /bin/bash 

CC=g++
#simulation binary
SIMUL=simulSpike

LDFLAGS = -lpthread -lfftw3

CXXFLAGS:=-g $(shell ls -1d ./src/*| sed 's/\.\/src/\-I\.\/src/')
FOLDERS:=$(shell ls  -1d ./src/*)
OBJ := $(shell ls ./src/*/*.cpp | sed 's/\/src\/\([a-zA-Z_]\+\)\/\([a-zA-Z_]*\).cpp/\/bin\/\2.o/') 
SRCS:=$(shell ls -1d ./src/* | sed 's/\/src\/\([a-zA-Z_]\+\)/\/src\/\1\/%.cpp/')

all: simulSpike

simulSpike:  ./main/mainSimul.cpp 
	@echo "Folders::" $(FOLDERS)
	@echo "Object::" $(OBJ)
	@echo "CF:" $(CXXFLAGS) 
	@echo "SRCS:" $(SRCS) 
	$(CC) $(CXXFLAGS) $(LDFLAGS)  ./main/mainSimul.cpp -o $@  $(OBJ)

./bin/%.o: $(SRCS) 
	echo "CF:" $(SRCS) 
	echo "tarteg:" $@
	$(CC) $(CXXFLAGS)  -c $< -o $@

	
clean: 
	rm bin/*.o

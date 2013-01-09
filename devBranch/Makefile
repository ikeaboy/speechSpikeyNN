CC=g++
#simulation binary
SIMUL=simulSpike

LDFLAGS = -lpthread -lfftw3

CXXFLAGS =  -I./processSound/ -I./soundInOut/ -I.
OBJ := $(subst ./soundInOut/,bin/,$(patsubst %.cpp,%.o,$(wildcard ./soundInOut/*.cpp)))

all: simulSpike

simulSpike: $(OBJ) ./main/mainSimul.cpp 
	@echo "Object::" $(OBJ) 
	$(CC) $(CXXFLAGS) $(LDFLAGS)    ./main/mainSimul.cpp -o $@  $(OBJ)

./bin/%.o: ./soundInOut/%.cpp ./soundInOut/soundInOut.h   
	 $(CC) $(CXXFLAGS)  -c $< -o $@ 
	 

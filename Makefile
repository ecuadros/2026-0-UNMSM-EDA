CXX = g++
CXXFLAGS = -g -Wall -Wextra -std=c++17

OBJ = main.o Vector.o
EXEC = main

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(OBJ)

main.o: main.cpp containers/Vector.h
	$(CXX) $(CXXFLAGS) -c main.cpp

Vector.o: Vector.cpp containers/Vector.h
	$(CXX) $(CXXFLAGS) -c Vector.cpp
clean:
	rm -f $(OBJ) $(EXEC)
.PHONY: all clean
CXX = g++
CXXFLAGS = -g -Wall -Wextra -std=c++17

OBJ = main.o DemoHeap.o
EXEC = main

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(OBJ)

main.o: main.cpp containers/Heap.h
	$(CXX) $(CXXFLAGS) -c main.cpp

DemoHeap.o: DemoHeap.cpp containers/Heap.h
	$(CXX) $(CXXFLAGS) -c DemoHeap.cpp
clean:
	rm -f $(OBJ) $(EXEC)
.PHONY: all clean
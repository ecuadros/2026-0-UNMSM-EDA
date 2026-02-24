CXX = g++
CXXFLAGS = -g -Wall -Wextra -std=c++17

OBJ = main.o DemoBinaryTree.o
EXEC = main

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(OBJ)

main.o: main.cpp containers/binarytree.h
	$(CXX) $(CXXFLAGS) -c main.cpp

DemoHeap.o: DemoBinaryTree.o containers/binarytree.h
	$(CXX) $(CXXFLAGS) -c DemoBinaryTree.cpp
clean:
	rm -f $(OBJ) $(EXEC)
.PHONY: all clean

CXX = g++
CXXFLAGS = -g -Wall -Wextra -std=c++17

OBJ = main.o DemoBTree.o
EXEC = main

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(OBJ)

main.o: main.cpp containers/BTree.h
	$(CXX) $(CXXFLAGS) -c main.cpp

DemoHeap.o: DemoBTree.o containers/BTree.h
	$(CXX) $(CXXFLAGS) -c DemoBTree.cpp
clean:
	rm -f $(OBJ) $(EXEC)
.PHONY: all clean

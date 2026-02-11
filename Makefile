CXX = g++
CXXFLAGS = -std=c++17 -Wall -g -pthread
LDFLAGS = -pthread

TARGET = main

SRCS = main.cpp util.cpp stack/DemoStack.cpp 
OBJS = main.o util.o DemoStack.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $^ -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

DemoStack.o: stack/DemoStack.cpp stack/Stack.h stack/Stack.cpp
	$(CXX) $(CXXFLAGS) -c stack/DemoStack.cpp -o DemoStack.o

clean:
	del /f *.o $(TARGET).exe

.PHONY: all clean
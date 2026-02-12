CXX = g++
CXXFLAGS = -std=c++17 -Wall -g -pthread
LDFLAGS = -pthread

TARGET = main
SRCS = main.cpp util.cpp pointers.cpp \
       DemoArray.cpp DemoLists.cpp DemoCircularLinkedList.cpp \
	   DemoDoubleLinkedList.cpp DemoCircularDoubleLinkedList.cpp \
	   algorithms/sorting.cpp
       DemoArray.cpp Demo.cpp \
	   sorting.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean

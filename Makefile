CXX = g++
CXXFLAGS = -std=c++17 -Wall -g -pthread
LDFLAGS = -pthread

TARGET = main.exe

    #DemoLists.cpp
    #sorting.cpp

SRCS = main.cpp util.cpp pointers.cpp DemoArray.cpp DemoDoubleLinkedLists.cpp DemoDoubleLinkedListsCircular.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)


.PHONY: all clean
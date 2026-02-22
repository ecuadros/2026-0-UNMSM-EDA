CXX = g++
CXXFLAGS = -std=c++17 -Wall -g -pthread # Añadido -pthread
LDFLAGS = -pthread # Añadido -pthread
DEBUGFLAGS = -g -O0 -DDEBUG

TARGET = main
SRCS = main.cpp util.cpp pointers.cpp \
       DemoArray.cpp DemoBinaryTree.cpp DemoAVLTree.cpp \
	   algorithms/sorting.cpp
OBJS = $(SRCS:.cpp=.o)

debug: CXXFLAGS += $(DEBUGFLAGS)
debug: $(TARGET)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean debug
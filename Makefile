CXX = g++
CXXFLAGS = -std=c++17 -Wall -g -pthread -I. -Icontainers
LDFLAGS = -pthread

TARGET = main
# Quita sorting.cpp de aquí
SRCS = main.cpp util.cpp pointers.cpp DemoArray.cpp DemoLists.cpp DemoCircular.cpp DemoLDE.cpp DemoEnlazadaCircular.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
CXX = g++
CXXFLAGS = -std=c++17 -Wall -g -pthread -I.
LDFLAGS = -pthread

TARGET = main
SRCS = main.cpp DemoQueue.cpp util.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	del /f *.o $(TARGET).exe 2>nul || rm -f $(OBJS) $(TARGET)

.PHONY: all clean
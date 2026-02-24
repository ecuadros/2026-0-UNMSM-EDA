CXX = g++
CXXFLAGS = -std=c++17 -Wall -g -pthread # Añadido -pthread
LDFLAGS = -pthread # Añadido -pthread

TARGET = main
SRCS = main.cpp util.cpp pointers.cpp \
       DemoArray.cpp DemoLists.cpp DemoCircularLists.cpp \
	   DemoDoubleLists.cpp DemoCircularDoubleLists.cpp \
       DemoStack.cpp DemoQueue.cpp DemoBinaryTree.cpp \
	   DemoAVL.cpp

	  # sorting.cpp
	  
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) *.txt

.PHONY: all clean
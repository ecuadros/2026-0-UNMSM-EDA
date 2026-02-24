CXX = g++

CXXFLAGS = -std=c++17 -Wall -g -pthread


TARGET = main


SRCS = main.cpp DemoHeap.cpp 

# Regla principal
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

# Limpiar
clean:
	rm -f $(TARGET)
	rm -f *.o

# Ejecutar
run: $(TARGET)
	./$(TARGET)
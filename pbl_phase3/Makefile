CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude

SRCS = src/Bin.cpp src/Vehicle.cpp src/Graph.cpp src/Route.cpp src/WasteManager.cpp src/main.cpp
TARGET = SmartWaste.exe

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

demo: $(TARGET)
	./$(TARGET) --demo

test: $(TARGET)
	./$(TARGET) --test

web: $(TARGET)
	python server.py

clean:
	rm -f $(TARGET)

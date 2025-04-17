CXX = g++
CXXFLAGS = -std=c++11 -Iinclude

SRC = main.cpp calculator.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = double_calc

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

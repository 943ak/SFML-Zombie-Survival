CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17
LIBS = -lsfml-graphics -lsfml-window -lsfml-system
TARGET = Zombie-Survival

all: $(TARGET)

$(TARGET): src/game.cpp
	$(CXX) $(CXXFLAGS) src/game.cpp -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)

.PHONY: all clean

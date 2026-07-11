TARGET = app

CXX = g++
CXXFLAGS = -O3 -Wall -Wextra

LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

SRCS = main.cpp
HEADERS = vector_editor.cpp

all: $(TARGET)

$(TARGET): $(SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run clean

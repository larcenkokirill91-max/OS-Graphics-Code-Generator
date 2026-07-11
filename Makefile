# Имя исполняемого файла
TARGET = app

# Компилятор и флаги
CXX = g++
CXXFLAGS = -O3 -Wall -Wextra

# Флаги линковки для библиотек SFML
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

# Исходные файлы проекта
SRCS = main.cpp
HEADERS = vector_editor.cpp

# Правило по умолчанию (срабатывает при вызове просто `make`)
all: $(TARGET)

# Сборка исполняемого файла
$(TARGET): $(SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS)

# Сборка и автоматический запуск программы (`make run`)
run: $(TARGET)
	./$(TARGET)

# Очистка скомпилированных файлов (`make clean`)
clean:
	rm -f $(TARGET)

# Указываем, что эти цели не являются реальными файлами
.PHONY: all run clean

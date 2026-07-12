# Имя исполняемого файла
TARGET = app
BUILD_DIR = build

# Правило по умолчанию: конфигурируем и собираем проект
all: $(BUILD_DIR)/Makefile
	cmake --build $(BUILD_DIR) --config Release

# Настройка конфигурации CMake (создание файлов сборки в папке build)
$(BUILD_DIR)/Makefile:
	cmake -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release

# Запуск скомпилированного приложения
run: all
	./$(BUILD_DIR)/$(TARGET)

# Полная очистка папки сборки и временных кэшей
clean:
	rm -rf $(BUILD_DIR) $(TARGET) AppDir *.AppImage

.PHONY: all run clean

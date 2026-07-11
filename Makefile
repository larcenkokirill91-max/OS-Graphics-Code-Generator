TARGET = app
BUILD_DIR = build

all: $(BUILD_DIR)/Makefile
	cmake --build $(BUILD_DIR) --config Release

$(BUILD_DIR)/Makefile:
	cmake -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release

run: all
	./$(BUILD_DIR)/$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET) AppDir *.AppImage

.PHONY: all run clean

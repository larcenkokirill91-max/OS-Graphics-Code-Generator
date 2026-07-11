cmake_minimum_required(VERSION 3.12)
project(OS_Graphics_Code_Generator VERSION 1.0.0 LANGUAGES CXX)

# Включаем стандарт C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Пытаемся автоматически найти установленную в системе библиотеку SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Указываем исполняемый файл и исходники
add_executable(app main.cpp)

# Подключаем заголовочные файлы и линкуем библиотеки SFML к нашему приложению
target_link_libraries(app PRIVATE sfml-graphics sfml-window sfml-system)

# Копируем папку со шрифтами в директорию сборки, чтобы приложение всегда видело их рядом
add_custom_command(TARGET app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/JetBrainsMono
    $<TARGET_FILE_DIR:app>/JetBrainsMono
)

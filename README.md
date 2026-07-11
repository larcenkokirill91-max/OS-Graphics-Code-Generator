# OS Graphics Code Generator (C++ Speed Edition) 🎨🚀

<p align="center">
  <a href="#english">English</a> • 
  <a href="#русский">Русский</a> • 
  <a href="#license">License</a>
</p>

---

<a name="english"></a>
## 🌍 Language / Язык : English

### 📝 About the Project
A high-performance interactive vector graphics editor built from scratch in C++ using **SFML** and **Pillow/PGI concepts**. It generates ready-to-use C/C++ rendering code specifically tailored for custom Operating System development (UEFI/Bare-metal environment).

Developed to prototype and generate pixel-perfect GUI layouts using optimized integer-based algorithms with custom Alpha Blending.

### 🔥 Key Features
* **Blazing Fast Rendering**: Written in native C++ for instantaneous multi-layered asset layout prototyping.
* **Pixel-Perfect OS Anti-Aliasing**: Embedded custom fixed-point integer arithmetic algorithm copying real-world bare-metal `draw_circle` behaviors.
* **Real-time Code Generation**: Seamlessly outputs standard procedural C graphics functions on the fly into an integrated IDE code preview pane.
* **Advanced GUI Control Panel**:
  * **Invisible Hitboxes**: Extended 30x30px slider grabbing zones for seamless mouse interactions.
  * **4-Channel Color Mixer**: Precise native sliders for Red, Green, Blue, and true Alpha transparency layer compositing.
  * **Adaptive Workspace Scrolling**: Web-styled dual scrollbars inside the IDE container utilizing standalone camera viewport bounds clipping.
* **Developer QoL**: Quick Clipboard export, live color test canvas preview, and step-by-step history tracebacks (`Undo`).

### 🛠 Tech Stack
* **Language:** C++17 (Optimized `-O3` architecture)
* **Framework:** SFML (Simple and Fast Multimedia Library)
* **Target OS Resolution:** Native 1280x1024 Workspace layout inside a flexible 1920x1024 rectangular application wrapper.

### 🚀 Getting Started
#### 📋 Prerequisites (Fedora Linux)
Ensure that you have `g++` build essentials and system `SFML` development libraries installed.
```bash
sudo dnf install gcc-c++ SFML-devel
```

#### 📂 Repository File Structure
```text
.
├── main.cpp              # Application lifecycle, Events & MSAA Viewport View
├── vector_editor.cpp     # Logic engine, AA-Circle fixed-point logic & C-generator
├── gui_elements.hpp      # Struct prototypes for mouse Sliders, Smooth Scrollbars & Buttons
├── Makefile              # Native GNU automated building tool
└── JetBrainsMono/        # Repository font bundle asset
```

#### 🏗️ Build and Execution
The project includes a ready-made automated GNU `Makefile`.
* **Compile and Run immediately:** `make run`
* **Compile executable (`app`) separately:** `make`
* **Clean workspace builds:** `make clean`

### 📌 Usage Guide
1. **Draw Area (Left)**: Hold Left Mouse Button (LMB) and drag over the light grey canvas area to sketch a shape.
2. **Tools & Color (Center)**: Set object properties (Rectangle/Circle types, RGBA coloring, or Contour stroke spacing) dynamically before drawing.
3. **IDE Code Hub (Right)**: Watch your code stack automatically build, manipulate the native scrollers to trace hidden lines, and press `Copy C-Code` to export everything instantly.

---

<a name="русский"></a>
## 🌍 Language / Язык : Русский

### 📝 О проекте
Высокопроизводительный интерактивный редактор векторной графики, написанный с нуля на C++ с использованием библиотеки **SFML** и концепций пиксельного альфа-смешивания. Он генерирует готовый к использованию код рендеринга на языке Си, специально разработанный для разработки графических подсистем кастомных операционных систем (в среде UEFI / Bare-metal).

Программа создана для быстрого прототипирования и генерации пиксельно-точных макетов интерфейса операционных систем с использованием оптимизированных целочисленных алгоритмов и честной прозрачности (Alpha Blending).

### 🔥 Ключевые особенности
* **Экстремальная скорость рендеринга**: Написан на нативном C++ для мгновенной отрисовки многослойных объектов и прозрачных элементов.
* **Пиксельно-точное сглаживание ОС**: Встроенный кастомный алгоритм вычислений с фиксированной запятой, в точности копирующий поведение Bare-metal функции `draw_circle` вашей операционной системы.
* **Генерация кода в реальном времени**: Мгновенно выводит стандартные процедурные функции рисования Си прямо во встроенное окно предварительного просмотра кода.
* **Продвинутая панель управления интерфейсом (GUI)**:
  * **Невидимые хитбоксы**: Расширенная зона захвата ползунков 30x30px для удобного перетаскивания мыши без точного прицеливания.
  * **4-канальный микшер цветов**: Нативные ползунки для точной настройки Красного, Зеленого, Синего каналов и Альфа-прозрачности слоев.
  * **Адаптивный скроллинг кода**: Классические тонкие скроллбары (вертикальный и горизонтальный) внутри IDE-панели, использующие изолированную камеру-вьюпорт для автоматического отсечения вылетающего текста.
* **Удобство для разработчика**: Быстрый экспорт кода в буфер обмена системы одной кнопкой, интерактивный холст предпросмотра и пошаговая отмена действий (`Undo`).

### 🛠 Технологический стек
* **Язык программирования:** C++17 (с флагами максимальной оптимизации скорости `-O3`)
* **Графическая библиотека:** SFML (Simple and Fast Multimedia Library)
* **Целевое разрешение холста ОС:** Фиксированное 1280x1024 внутри прямоугольного окна приложения размером 1920x1024.

### 🚀 С чего начать
#### 📋 Предварительные требования (Fedora Linux)
Убедитесь, что в вашей системе установлены инструменты сборки `g++` и пакет разработки `SFML`.
```bash
sudo dnf install gcc-c++ SFML-devel
```

#### 📂 Структура файлов проекта
```text
.
├── main.cpp              # Главный цикл приложения, события, MSAA-сглаживание и вьюпорт
├── vector_editor.cpp     # Движок логики, целочисленная математика AA-Circle и генератор Си-кода
├── gui_elements.hpp      # Шаблоны и структуры для ползунков, кнопок и скроллбаров
├── Makefile              # Скрипт автоматизации сборки GNU
└── JetBrainsMono/        # Папка со шрифтом проекта
```

#### 🏗️ Сборка и запуск
Проект включает в себя готовый `Makefile` для автоматизации.
* **Скомпилировать и сразу запустить программу:** `make run`
* **Скомпилировать только исполняемый файл (`app`):** `make`
* **Очистить папку от скомпилированных файлов:** `make clean`

### 📌 Инструкция по использованию
1. **Область рисования (Слева)**: Зажмите левую кнопку мыши (ЛКМ) и тяните по светло-серому холсту, чтобы наметить контуры будущей фигуры.
2. **Инструменты и цвет (В центре)**: Настраивайте параметры объектов (выбирайте прямоугольник/круг, смешивайте цвета RGBA или выставляйте толщину контура) прямо перед рисованием.
3. **IDE-панель кода (Справа)**: Наблюдайте, как код автоматически пишется в черном окошке. Перетаскивайте мышкой тонкие полосы скроллбаров, если строк много, и жмите `Copy C-Code`, чтобы мгновенно скопировать готовый Си-код в буфер обмена операционной системы.

---

<a name="license"></a>
## 📄 License

This project is licensed under the MIT License - see the LICENSE file for details.

import tkinter as tk
from tkinter import ttk
from tkinter import colorchooser
from PIL import Image, ImageDraw, ImageTk

class OsVectorEditor:
    def __init__(self, root):
        self.root = root
        self.root.title("OS Graphics Code Generator (1280x1024)")
        self.root.geometry("1600x900")

        # Новое разрешение экрана вашей ОС
        self.WIDTH = 1280
        self.HEIGHT = 1024

        # Выбранный цвет по умолчанию (Черный в формате RGB и HEX)
        self.current_rgb = (0, 0, 0)
        self.current_alpha = 255 
        self.current_hex = "#000000"

        # Настройки инструментов рисования
        self.current_tool = "rect"  
        self.start_x = None
        self.start_y = None
        self.preview_shape = None

        # Список для хранения сгенерированного кода каждой фигуры
        self.shapes_code = []

        self.create_widgets()
        self.update_code_text()

    def create_widgets(self):
        # Главный контейнер (разделение на Лево и Право)
        main_paned = ttk.PanedWindow(self.root, orient=tk.HORIZONTAL)
        main_paned.pack(fill=tk.BOTH, expand=True)

        # ================= ЛЕВАЯ ПАНЕЛЬ: ВЫВОД КОДА =================
        left_frame = ttk.Frame(main_paned, padding=10)
        main_paned.add(left_frame, weight=1)

        code_label = ttk.Label(left_frame, text="Генерируемый C-код:", font=("Arial", 11, "bold"))
        code_label.pack(anchor=tk.W, pady=(0, 5))

        # Текстовое поле с прокруткой для вывода Си-кода
        self.code_text = tk.Text(left_frame, font=("Courier New", 10), wrap=tk.NONE)
        scrollbar_y = ttk.Scrollbar(left_frame, orient=tk.VERTICAL, command=self.code_text.yview)
        scrollbar_x = ttk.Scrollbar(left_frame, orient=tk.HORIZONTAL, command=self.code_text.xview)
        self.code_text.configure(yscrollcommand=scrollbar_y.set, xscrollcommand=scrollbar_x.set)

        scrollbar_y.pack(side=tk.RIGHT, fill=tk.Y)
        scrollbar_x.pack(side=tk.BOTTOM, fill=tk.X)
        self.code_text.pack(fill=tk.BOTH, expand=True)

        # ================= ПРАВАЯ ПАНЕЛЬ: ХОЛСТ И ИНСТРУМЕНТЫ =================
        right_frame = ttk.Frame(main_paned, padding=10)
        main_paned.add(right_frame, weight=2)

        # Верхняя панель управления (Инструменты, Цвет, Прозрачность, Очистка)
        toolbar = ttk.Frame(right_frame)
        toolbar.pack(fill=tk.X, pady=(0, 10))

        # Выбор фигуры
        ttk.Label(toolbar, text="Фигура: ").pack(side=tk.LEFT)
        self.tool_var = tk.StringVar(value="rect")
        
        rect_btn = ttk.Radiobutton(toolbar, text="Прямоугольник", variable=self.tool_var, value="rect", command=self.change_tool)
        rect_btn.pack(side=tk.LEFT, padx=5)
        
        circle_btn = ttk.Radiobutton(toolbar, text="Круг", variable=self.tool_var, value="circle", command=self.change_tool)
        circle_btn.pack(side=tk.LEFT, padx=5)

        # Выбор цвета
        ttk.Label(toolbar, text=" |  Цвет: ").pack(side=tk.LEFT)
        self.color_preview = tk.Frame(toolbar, width=25, height=25, bg=self.current_hex, relief=tk.RAISED, bd=2)
        self.color_preview.pack(side=tk.LEFT, padx=5)
        
        color_btn = ttk.Button(toolbar, text="Выбрать цвет", command=self.choose_color)
        color_btn.pack(side=tk.LEFT, padx=5)

        # Полоска для альфа-канала (прозрачности)
        ttk.Label(toolbar, text=" |  Прозрачность (Alpha): ").pack(side=tk.LEFT)
        self.alpha_scale = ttk.Scale(toolbar, from_=0, to=255, orient=tk.HORIZONTAL, command=self.update_alpha)
        self.alpha_scale.set(255)
        self.alpha_scale.pack(side=tk.LEFT, padx=5)
        
        self.alpha_label = ttk.Label(toolbar, text="255")
        self.alpha_label.pack(side=tk.LEFT, padx=2)

        # Кнопка очистки холста
        clear_btn = ttk.Button(toolbar, text="Очистить всё", command=self.clear_canvas)
        clear_btn.pack(side=tk.RIGHT)

        # Скроллбары для холста
        canvas_container = ttk.Frame(right_frame)
        canvas_container.pack(fill=tk.BOTH, expand=True)

        canvas_scroll_y = ttk.Scrollbar(canvas_container, orient=tk.VERTICAL)
        canvas_scroll_x = ttk.Scrollbar(canvas_container, orient=tk.HORIZONTAL)

        # Холст для рисования фиксированного размера 1280 на 1024
        self.canvas = tk.Canvas(
            canvas_container, 
            width=self.WIDTH, 
            height=self.HEIGHT, 
            bg="#F0F0F0", 
            cursor="cross",
            scrollregion=(0, 0, self.WIDTH, self.HEIGHT),
            yscrollcommand=canvas_scroll_y.set,
            xscrollcommand=canvas_scroll_x.set
        )
        
        canvas_scroll_y.config(command=self.canvas.yview)
        canvas_scroll_x.config(command=self.canvas.xview)

        canvas_scroll_y.pack(side=tk.RIGHT, fill=tk.Y)
        canvas_scroll_x.pack(side=tk.BOTTOM, fill=tk.X)
        self.canvas.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

        # Инициализация фонового изображения PIL для поддержки прозрачности
        self.pil_image = Image.new("RGBA", (self.WIDTH, self.HEIGHT), (240, 240, 240, 255)) # Цвет #F0F0F0
        self.tk_image = ImageTk.PhotoImage(self.pil_image)
        self.canvas_image_id = self.canvas.create_image(0, 0, anchor=tk.NW, image=self.tk_image)

        # Привязка мыши к холсту
        self.canvas.bind("<ButtonPress-1>", self.on_draw_start)
        self.canvas.bind("<B1-Motion>", self.on_drawing)
        self.canvas.bind("<ButtonRelease-1>", self.on_draw_end)

    def change_tool(self):
        self.current_tool = self.tool_var.get()

    def choose_color(self):
        color_code = colorchooser.askcolor(title="Выберите цвет для графики ОС")
        if color_code and color_code[0]:
            self.current_rgb = tuple(map(int, color_code[0])) 
            self.current_hex = color_code[1]
            self.color_preview.config(bg=self.current_hex)

    def update_alpha(self, val):
        self.current_alpha = int(float(val))
        self.alpha_label.config(text=str(self.current_alpha))

    def update_code_text(self):
        self.code_text.delete("1.0", tk.END)
        code = (
            f"// === ПРОТОТИПЫ ФУНКЦИЙ ВАШЕЙ ОС ===\n"
            f"// void draw_pixel(UINT32 x, UINT32 y, UINT8 r, UINT8 g, UINT8 b, UINT8 alpha);\n"
            f"// void draw_rect(UINT32 x, UINT32 y, UINT32 w, UINT32 h, UINT8 r, UINT8 g, UINT8 b, UINT8 alpha);\n"
            f"// void draw_circle(UINT32 center_x, UINT32 center_y, UINT32 rad, UINT8 r, UINT8 g, UINT8 b, UINT8 a);\n\n"
            f"#define SCREEN_WIDTH  {self.WIDTH}\n"
            f"#define SCREEN_HEIGHT {self.HEIGHT}\n\n"
            f"void render_os_vector_graphics() {{\n"
        )
        for line in self.shapes_code:
            code += f"    {line}\n"
        code += "}"
        self.code_text.insert(tk.END, code)

    def clear_canvas(self):
        self.shapes_code.clear()
        self.update_code_text()
        
        # Сбрасываем изображение PIL к базовому серому фону
        self.pil_image = Image.new("RGBA", (self.WIDTH, self.HEIGHT), (240, 240, 240, 255))
        self.tk_image = ImageTk.PhotoImage(self.pil_image)
        self.canvas.itemconfig(self.canvas_image_id, image=self.tk_image)
        
        # Удаляем только временные элементы (например, линии предпросмотра)
        for item in self.canvas.find_all():
            if item != self.canvas_image_id:
                self.canvas.delete(item)

    def get_canvas_xy(self, event):
        x = int(self.canvas.canvasx(event.x))
        y = int(self.canvas.canvasy(event.y))
        return x, y

    def on_draw_start(self, event):
        self.start_x, self.start_y = self.get_canvas_xy(event)
        if self.current_tool == "rect":
            self.preview_shape = self.canvas.create_rectangle(self.start_x, self.start_y, self.start_x, self.start_y, outline="black", dash=(4, 4))
        elif self.current_tool == "circle":
            self.preview_shape = self.canvas.create_oval(self.start_x, self.start_y, self.start_x, self.start_y, outline="black", dash=(4, 4))

    def on_drawing(self, event):
        cur_x, cur_y = self.get_canvas_xy(event)
        self.canvas.coords(self.preview_shape, self.start_x, self.start_y, cur_x, cur_y)

    def on_draw_end(self, event):
        end_x, end_y = self.get_canvas_xy(event)
        self.canvas.delete(self.preview_shape)

        if abs(self.start_x - end_x) < 2 and abs(self.start_y - end_y) < 2:
            return

        r, g, b = self.current_rgb
        a = self.current_alpha

        # Создаем прозрачный оверлей текущего размера экрана для рендеринга новой фигуры
        overlay = Image.new("RGBA", (self.WIDTH, self.HEIGHT), (0, 0, 0, 0))
        draw = ImageDraw.Draw(overlay)

        if self.current_tool == "rect":
            x = min(self.start_x, end_x)
            y = min(self.start_y, end_y)
            w = abs(self.start_x - end_x)
            h = abs(self.start_y - end_y)

            # Рисуем фигуру на прозрачном слое оверлея
            draw.rectangle([x, y, x + w, y + h], fill=(r, g, b, a))
            # Математически смешиваем слои по формуле альфа-канала
            self.pil_image = Image.alpha_composite(self.pil_image, overlay)
            
            code_line = f"draw_rect({x}, {y}, {w}, {h}, {r}, {g}, {b}, {a});"
            self.shapes_code.append(code_line)

        elif self.current_tool == "circle":
            center_x = int((self.start_x + end_x) / 2)
            center_y = int((self.start_y + end_y) / 2)
            rad = int((abs(self.start_x - end_x) + abs(self.start_y - end_y)) / 4)

            x1, y1 = center_x - rad, center_y - rad
            x2, y2 = center_x + rad, center_y + rad
            
            # Рисуем круг на слое оверлея
            draw.ellipse([x1, y1, x2, y2], fill=(r, g, b, a))
            # Смешиваем слои
            self.pil_image = Image.alpha_composite(self.pil_image, overlay)
            
            code_line = f"draw_circle({center_x}, {center_y}, {rad}, {r}, {g}, {b}, {a});"
            self.shapes_code.append(code_line)

        self.tk_image = ImageTk.PhotoImage(self.pil_image)
        self.canvas.itemconfig(self.canvas_image_id, image=self.tk_image)

        self.update_code_text()

if __name__ == "__main__":
    root = tk.Tk()
    app = OsVectorEditor(root)
    root.mainloop()

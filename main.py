import tkinter as tk
from tkinter import ttk
from tkinter import colorchooser
from PIL import ImageTk
from vector_editor import OsVectorEditor

class OsVectorEditorGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("OS Graphics Code Generator (1280x1024)")
        self.root.geometry("1600x900")

        # Инициализируем логический движок редактора
        self.editor = OsVectorEditor()

        self.preview_shape = None
        self.start_x = None
        self.start_y = None

        self.create_widgets()
        self.refresh_ui()

    def create_widgets(self):
        # Главный контейнер (разделение на Лево и Право)
        main_paned = ttk.PanedWindow(self.root, orient=tk.HORIZONTAL)
        main_paned.pack(fill=tk.BOTH, expand=True)

        # ================= ЛЕВАЯ ПАНЕЛЬ: ВЫВОД КОДА =================
        left_frame = ttk.Frame(main_paned, padding=10)
        main_paned.add(left_frame, weight=1)

        code_label = ttk.Label(left_frame, text="Генерируемый C-код:", font=("Arial", 11, "bold"))
        code_label.pack(anchor=tk.W, pady=(0, 5))

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
        self.color_preview = tk.Frame(toolbar, width=25, height=25, bg=self.editor.current_hex, relief=tk.RAISED, bd=2)
        self.color_preview.pack(side=tk.LEFT, padx=5)
        
        color_btn = ttk.Button(toolbar, text="Выбрать цвет", command=self.choose_color)
        color_btn.pack(side=tk.LEFT, padx=5)

        # Полоска для альфа-канала
        ttk.Label(toolbar, text=" |  Прозрачность (Alpha): ").pack(side=tk.LEFT)
        self.alpha_scale = ttk.Scale(toolbar, from_=0, to=255, orient=tk.HORIZONTAL, command=self.update_alpha)
        self.alpha_scale.set(255)
        self.alpha_scale.pack(side=tk.LEFT, padx=5)
        
        self.alpha_label = ttk.Label(toolbar, text="255")
        self.alpha_label.pack(side=tk.LEFT, padx=2)

        # Кнопки управления
        clear_btn = ttk.Button(toolbar, text="Очистить всё", command=self.clear_canvas)
        clear_btn.pack(side=tk.RIGHT, padx=2)

        undo_btn = ttk.Button(toolbar, text="Отменить (Undo)", command=self.undo_last_action)
        undo_btn.pack(side=tk.RIGHT, padx=2)

        # Скроллбары для холста
        canvas_container = ttk.Frame(right_frame)
        canvas_container.pack(fill=tk.BOTH, expand=True)

        canvas_scroll_y = ttk.Scrollbar(canvas_container, orient=tk.VERTICAL)
        canvas_scroll_x = ttk.Scrollbar(canvas_container, orient=tk.HORIZONTAL)

        self.canvas = tk.Canvas(
            canvas_container, 
            width=self.editor.WIDTH, 
            height=self.editor.HEIGHT, 
            bg="#F0F0F0", 
            cursor="cross",
            scrollregion=(0, 0, self.editor.WIDTH, self.editor.HEIGHT),
            yscrollcommand=canvas_scroll_y.set,
            xscrollcommand=canvas_scroll_x.set
        )
        
        canvas_scroll_y.config(command=self.canvas.yview)
        canvas_scroll_x.config(command=self.canvas.xview)

        canvas_scroll_y.pack(side=tk.RIGHT, fill=tk.Y)
        canvas_scroll_x.pack(side=tk.BOTTOM, fill=tk.X)
        self.canvas.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

        # Инициализация изображения на холсте
        self.tk_image = ImageTk.PhotoImage(self.editor.pil_image)
        self.canvas_image_id = self.canvas.create_image(0, 0, anchor=tk.NW, image=self.tk_image)

        # Привязка мыши к холсту
        self.canvas.bind("<ButtonPress-1>", self.on_draw_start)
        self.canvas.bind("<B1-Motion>", self.on_drawing)
        self.canvas.bind("<ButtonRelease-1>", self.on_draw_end)

    def change_tool(self):
        self.editor.current_tool = self.tool_var.get()

    def choose_color(self):
        color_code = colorchooser.askcolor(title="Выберите цвет для графики ОС")
        if color_code and color_code[1]:
            self.editor.current_rgb = tuple(map(int, color_code[0])) 
            self.editor.current_hex = color_code[1]
            self.color_preview.config(bg=self.editor.current_hex)

    def update_alpha(self, val):
        self.editor.current_alpha = int(float(val))
        self.alpha_label.config(text=str(self.editor.current_alpha))

    def refresh_ui(self):
        """Синхронизирует холст Tkinter и текстовое поле с логическим ядром"""
        self.tk_image = ImageTk.PhotoImage(self.editor.pil_image)
        self.canvas.itemconfig(self.canvas_image_id, image=self.tk_image)
        
        self.code_text.delete("1.0", tk.END)
        self.code_text.insert(tk.END, self.editor.get_code_string())

    def undo_last_action(self):
        self.editor.undo()
        self.refresh_ui()

    def clear_canvas(self):
        self.editor.clear()
        self.refresh_ui()
        for item in self.canvas.find_all():
            if item != self.canvas_image_id:
                self.canvas.delete(item)

    def get_canvas_xy(self, event):
        x = int(self.canvas.canvasx(event.x))
        y = int(self.canvas.canvasy(event.y))
        return x, y

    def on_draw_start(self, event):
        self.start_x, self.start_y = self.get_canvas_xy(event)
        if self.editor.current_tool == "rect":
            self.preview_shape = self.canvas.create_rectangle(self.start_x, self.start_y, self.start_x, self.start_y, outline="black", dash=(4, 4))
        elif self.editor.current_tool == "circle":
            self.preview_shape = self.canvas.create_oval(self.start_x, self.start_y, self.start_x, self.start_y, outline="black", dash=(4, 4))

    def on_drawing(self, event):
        cur_x, cur_y = self.get_canvas_xy(event)
        self.canvas.coords(self.preview_shape, self.start_x, self.start_y, cur_x, cur_y)

    def on_draw_end(self, event):
        end_x, end_y = self.get_canvas_xy(event)
        self.canvas.delete(self.preview_shape)
        
        if abs(self.start_x - end_x) >= 2 or abs(self.start_y - end_y) >= 2:
            self.editor.add_shape(self.start_x, self.start_y, end_x, end_y)
            self.refresh_ui()

if __name__ == "__main__":
    root = tk.Tk()
    app = OsVectorEditorGUI(root)
    root.mainloop()

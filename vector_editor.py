from PIL import Image, ImageDraw

class OsVectorEditor:
    def __init__(self):
        # Разрешение экрана вашей ОС
        self.WIDTH = 1280
        self.HEIGHT = 1024

        # Состояние инструментов (управляется из GUI)
        self.current_rgb = (0, 0, 0)
        self.current_alpha = 255 
        self.current_hex = "#000000"
        self.current_tool = "rect"  

        # История фигур для генерации кода и отмены действий (Undo)
        self.shapes_data = []
        self.shapes_code = []

        # Базовый фоновый слой Pillow
        self.pil_image = Image.new("RGBA", (self.WIDTH, self.HEIGHT), (240, 240, 240, 255))

    def get_code_string(self):
        """Формирует финальный текст С-кода из накопленных строк"""
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
        return code

    def add_shape(self, start_x, start_y, end_x, end_y):
        """Просчитывает геометрию новой фигуры и накладывает её на слой по альфа-каналу"""
        r, g, b = self.current_rgb
        a = self.current_alpha

        overlay = Image.new("RGBA", (self.WIDTH, self.HEIGHT), (0, 0, 0, 0))
        draw = ImageDraw.Draw(overlay)

        if self.current_tool == "rect":
            x = min(start_x, end_x)
            y = min(start_y, end_y)
            w = abs(start_x - end_x)
            h = abs(start_y - end_y)

            coords = [x, y, x + w, y + h]
            draw.rectangle(coords, fill=(r, g, b, a))
            
            code_line = f"draw_rect({x}, {y}, {w}, {h}, {r}, {g}, {b}, {a});"
            self.shapes_code.append(code_line)
            self.shapes_data.append({'type': 'rect', 'coords': coords, 'rgba': (r, g, b, a)})

        elif self.current_tool == "circle":
            center_x = int((start_x + end_x) / 2)
            center_y = int((start_y + end_y) / 2)
            rad = int((abs(start_x - end_x) + abs(start_y - end_y)) / 4)

            x1, y1 = center_x - rad, center_y - rad
            x2, y2 = center_x + rad, center_y + rad
            
            coords = [x1, y1, x2, y2]
            draw.ellipse(coords, fill=(r, g, b, a))
            
            code_line = f"draw_circle({center_x}, {center_y}, {rad}, {r}, {g}, {b}, {a});"
            self.shapes_code.append(code_line)
            self.shapes_data.append({'type': 'circle', 'coords': coords, 'rgba': (r, g, b, a)})

        # Математическое смешивание нового элемента с накопленной графикой
        self.pil_image = Image.alpha_composite(self.pil_image, overlay)

    def redraw_all(self):
        """Полная пересборка холста с нуля (после очистки или Undo)"""
        self.pil_image = Image.new("RGBA", (self.WIDTH, self.HEIGHT), (240, 240, 240, 255))
        
        for shape in self.shapes_data:
            overlay = Image.new("RGBA", (self.WIDTH, self.HEIGHT), (0, 0, 0, 0))
            draw = ImageDraw.Draw(overlay)
            
            if shape['type'] == 'rect':
                draw.rectangle(shape['coords'], fill=shape['rgba'])
            elif shape['type'] == 'circle':
                draw.ellipse(shape['coords'], fill=shape['rgba'])
                
            self.pil_image = Image.alpha_composite(self.pil_image, overlay)

    def undo(self):
        """Откат последней операции"""
        if self.shapes_data:
            self.shapes_data.pop()
            self.shapes_code.pop()
            self.redraw_all()

    def clear(self):
        """Полный сброс истории"""
        self.shapes_data.clear()
        self.shapes_code.clear()
        self.redraw_all()

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <iostream>

const unsigned int CANVAS_WIDTH = 1280;
const unsigned int CANVAS_HEIGHT = 1024;

enum Tool { RECTANGLE, CIRCLE };

struct ShapeData {
    Tool type;
    int x1, y1, x2, y2;
    sf::Color color;
    std::string c_code;
};

class OsVectorEditor {
private:
    std::string generate_shape_c_code(const ShapeData& shape) {
        std::ostringstream ss;
        if (shape.type == RECTANGLE) {
            int x = std::min(shape.x1, shape.x2);
            int y = std::min(shape.y1, shape.y2);
            int w = std::abs(shape.x1 - shape.x2);
            int h = std::abs(shape.y1 - shape.y2);
            ss << "draw_rect(" << x << ", " << y << ", " << w << ", " << h << ", "
               << (int)shape.color.r << ", " << (int)shape.color.g << ", " << (int)shape.color.b << ", " << (int)shape.color.a << ");";
        } else if (shape.type == CIRCLE) {
            int center_x = (shape.x1 + shape.x2) / 2;
            int center_y = (shape.y1 + shape.y2) / 2;
            int rad = (std::abs(shape.x1 - shape.x2) + std::abs(shape.y1 - shape.y2)) / 4;
            ss << "draw_circle(" << center_x << ", " << center_y << ", " << rad << ", "
               << (int)shape.color.r << ", " << (int)shape.color.g << ", " << (int)shape.color.b << ", " << (int)shape.color.a << ");";
        }
        return ss.str();
    }

    void draw_rect_fast(sf::Uint8* pixels, int x1, int y1, int x2, int y2, sf::Color color) {
        int start_x = std::max(0, std::min(x1, x2));
        int end_x = std::min((int)CANVAS_WIDTH - 1, std::max(x1, x2));
        int start_y = std::max(0, std::min(y1, y2));
        int end_y = std::min((int)CANVAS_HEIGHT - 1, std::max(y1, y2));

        float alpha = color.a / 255.0f;
        float inv_alpha = 1.0f - alpha;

        for (int y = start_y; y <= end_y; ++y) {
            for (int x = start_x; x <= end_x; ++x) {
                int idx = (y * CANVAS_WIDTH + x) * 4;
                pixels[idx]     = (sf::Uint8)(color.r * alpha + pixels[idx]     * inv_alpha);
                pixels[idx + 1] = (sf::Uint8)(color.g * alpha + pixels[idx + 1] * inv_alpha);
                pixels[idx + 2] = (sf::Uint8)(color.b * alpha + pixels[idx + 2] * inv_alpha);
                pixels[idx + 3] = (sf::Uint8)(255 * alpha + pixels[idx + 3] * inv_alpha);
            }
        }
    }

    void draw_aa_circle_fast(sf::Uint8* pixels, int center_x, int center_y, int rad, sf::Color color) {
        if (rad == 0) return;

        int start_y = (center_y >= rad) ? (center_y - rad) : 0;
        int end_y = std::min((int)CANVAS_HEIGHT - 1, center_y + rad);

        int start_x_bound = (center_x >= rad) ? (center_x - rad) : 0;
        int end_x_bound = std::min((int)CANVAS_WIDTH - 1, center_x + rad);

        int rad_scaled = rad * 256;
        int rad_sq_scaled = rad_scaled * rad_scaled;

        for (int cur_y = start_y; cur_y <= end_y; cur_y++) {
            int dy = (cur_y > center_y) ? (cur_y - center_y) : (center_y - cur_y);
            int dy_scaled = dy * 256;
            int dy_sq = dy_scaled * dy_scaled;

            for (int cur_x = start_x_bound; cur_x <= end_x_bound; cur_x++) {
                int dx = (cur_x > center_x) ? (cur_x - center_x) : (center_x - cur_x);
                int dx_scaled = dx * 256;
                int dist_sq = (dx_scaled * dx_scaled) + dy_sq;

                if (dist_sq <= rad_sq_scaled) {
                    int dist = 0;
                    while ((dist + 1) * (dist + 1) <= dist_sq) {
                        dist++;
                    }

                    int inner_edge = rad_scaled - 256;
                    sf::Color src_color = color;

                    if (dist > inner_edge) {
                        int edge_dist = dist - inner_edge;
                        int alpha_factor = 256 - edge_dist;
                        src_color.a = (color.a * alpha_factor) >> 8;
                    }

                    if (src_color.a > 0) {
                        int idx = (cur_y * CANVAS_WIDTH + cur_x) * 4;
                        float alpha = src_color.a / 255.0f;
                        float inv_alpha = 1.0f - alpha;

                        pixels[idx]     = (sf::Uint8)(src_color.r * alpha + pixels[idx]     * inv_alpha);
                        pixels[idx + 1] = (sf::Uint8)(src_color.g * alpha + pixels[idx + 1] * inv_alpha);
                        pixels[idx + 2] = (sf::Uint8)(src_color.b * alpha + pixels[idx + 2] * inv_alpha);
                        pixels[idx + 3] = (sf::Uint8)(255 * alpha + pixels[idx + 3] * inv_alpha);
                    }
                }
            }
        }
    }

public:
    std::vector<ShapeData> shapes_history;
    sf::Uint8* canvas_pixels;

    OsVectorEditor() {
        canvas_pixels = new sf::Uint8[CANVAS_WIDTH * CANVAS_HEIGHT * 4];
        clear();
    }

    ~OsVectorEditor() {
        delete[] canvas_pixels;
    }

    void add_shape(Tool tool, int x1, int y1, int x2, int y2, sf::Color color) {
        ShapeData new_shape;
        new_shape.type = tool;
        new_shape.x1 = x1;
        new_shape.y1 = y1;
        new_shape.x2 = x2;
        new_shape.y2 = y2;
        new_shape.color = color;
        new_shape.c_code = generate_shape_c_code(new_shape);

        shapes_history.push_back(new_shape);
        redraw_all();
    }

    void redraw_all() {
        // Заполняем фон текстуры дефолтным серым цветом #F0F0F0, полностью обнуляя альфу для прозрачного блендинга
        for (unsigned int i = 0; i < CANVAS_WIDTH * CANVAS_HEIGHT * 4; i += 4) {
            canvas_pixels[i]     = 240;
            canvas_pixels[i + 1] = 240;
            canvas_pixels[i + 2] = 240;
            canvas_pixels[i + 3] = 0; 
        }

        for (const auto& shape : shapes_history) {
            if (shape.type == RECTANGLE) {
                draw_rect_fast(canvas_pixels, shape.x1, shape.y1, shape.x2, shape.y2, shape.color);
            } else if (shape.type == CIRCLE) {
                int center_x = (shape.x1 + shape.x2) / 2;
                int center_y = (shape.y1 + shape.y2) / 2;
                int rad = (std::abs(shape.x1 - shape.x2) + std::abs(shape.y1 - shape.y2)) / 4;
                draw_aa_circle_fast(canvas_pixels, center_x, center_y, rad, shape.color);
            }
        }

        // Накладываем итоговый буфер на непрозрачную подложку, чтобы SFML корректно отображал окно
        for (unsigned int i = 0; i < CANVAS_WIDTH * CANVAS_HEIGHT * 4; i += 4) {
            if (canvas_pixels[i + 3] < 255) {
                float alpha = canvas_pixels[i + 3] / 255.0f;
                float inv_alpha = 1.0f - alpha;
                canvas_pixels[i]     = (sf::Uint8)(canvas_pixels[i] * alpha + 240 * inv_alpha);
                canvas_pixels[i + 1] = (sf::Uint8)(canvas_pixels[i + 1] * alpha + 240 * inv_alpha);
                canvas_pixels[i + 2] = (sf::Uint8)(canvas_pixels[i + 2] * alpha + 240 * inv_alpha);
                canvas_pixels[i + 3] = 255;
            }
        }
    }

    void undo() {
        if (!shapes_history.empty()) {
            shapes_history.pop_back();
            redraw_all();
        }
    }

    void clear() {
        shapes_history.clear();
        redraw_all();
    }

    void print_c_code() {
        std::system("cls || clear");
        std::cout << "// === ПРОТОТИПЫ ФУНКЦИЙ ВАШЕЙ ОС ===\n"
                  << "// void draw_pixel(UINT32 x, UINT32 y, UINT8 r, UINT8 g, UINT8 b, UINT8 alpha);\n"
                  << "// void draw_rect(UINT32 x, UINT32 y, UINT32 w, UINT32 h, UINT8 r, UINT8 g, UINT8 b, UINT8 alpha);\n"
                  << "// void draw_circle(UINT32 center_x, UINT32 center_y, UINT32 rad, UINT8 r, UINT8 g, UINT8 b, UINT8 a);\n\n"
                  << "#define SCREEN_WIDTH  " << CANVAS_WIDTH << "\n"
                  << "#define SCREEN_HEIGHT " << CANVAS_HEIGHT << "\n\n"
                  << "void render_os_vector_graphics() {\n";
        for (const auto& shape : shapes_history) {
            std::cout << "    " << shape.c_code << "\n";
        }
        std::cout << "}\n";
    }
};

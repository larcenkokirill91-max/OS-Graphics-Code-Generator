#include <SFML/Graphics.hpp>
#include "vector_editor.cpp" // Логика движка
#include "gui_elements.hpp"  // Элементы интерфейса (Слайдеры и Кнопки)

int main() {
    sf::RenderWindow window(sf::VideoMode(1600, 1024), "OS Graphics Code Generator (C++ Speed Edition)");
    window.setFramerateLimit(60);

    OsVectorEditor editor;

    sf::Texture canvas_texture;
    canvas_texture.create(CANVAS_WIDTH, CANVAS_HEIGHT);
    canvas_texture.update(editor.canvas_pixels);
    sf::Sprite canvas_sprite(canvas_texture);

    Tool active_tool = RECTANGLE;
    int color_R = 0, color_G = 0, color_B = 0, color_A = 255;

    bool is_drawing = false;
    int mouse_start_x = 0, mouse_start_y = 0;
    int mouse_curr_x = 0, mouse_curr_y = 0;

    sf::Font font;
    font.loadFromFile("arial.ttf"); 

    // Инициализация элементов интерфейса из gui_elements.hpp
    Slider sliderR, sliderG, sliderB, sliderA;
    sliderR.init(1320.f, 200.f, 240.f, 0, 255, &color_R, "Красный (R)", sf::Color::Red);
    sliderG.init(1320.f, 250.f, 240.f, 0, 255, &color_G, "Зеленый (G)", sf::Color::Green);
    sliderB.init(1320.f, 300.f, 240.f, 0, 255, &color_B, "Синий (B)", sf::Color::Blue);
    sliderA.init(1320.f, 350.f, 240.f, 0, 255, &color_A, "Прозрачность (Alpha)", sf::Color(100, 100, 100));

    Button btnRect, btnCircle, btnUndo, btnClear;
    btnRect.init(1320.f, 50.f, 115.f, 35.f, "Прямоугольник", font);
    btnCircle.init(1445.f, 50.f, 115.f, 35.f, "Круг", font);
    btnUndo.init(1320.f, 430.f, 115.f, 35.f, "Отменить", font);
    btnClear.init(1445.f, 430.f, 115.f, 35.f, "Очистить", font);

    sf::RectangleShape color_preview_box(sf::Vector2f(240.f, 30.f));
    color_preview_box.setPosition(1320.f, 120.f);
    color_preview_box.setOutlineColor(sf::Color::Black);
    color_preview_box.setOutlineThickness(1.f);

    sf::Text textLabels[4];
    for(int i = 0; i < 4; ++i) {
        textLabels[i].setFont(font);
        textLabels[i].setCharacterSize(13);
        textLabels[i].setFillColor(sf::Color::Black);
    }

    sf::Text panel_title;
    panel_title.setFont(font);
    panel_title.setString("ИНСТРУМЕНТЫ И ЦВЕТ");
    panel_title.setCharacterSize(16);
    panel_title.setFillColor(sf::Color(50, 50, 50));
    panel_title.setPosition(1320.f, 15.f);

    editor.print_c_code();

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            sliderR.handleEvent(event, window);
            sliderG.handleEvent(event, window);
            sliderB.handleEvent(event, window);
            sliderA.handleEvent(event, window);

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                if (btnRect.isClicked(window)) active_tool = RECTANGLE;
                if (btnCircle.isClicked(window)) active_tool = CIRCLE;
                if (btnUndo.isClicked(window)) {
                    editor.undo();
                    canvas_texture.update(editor.canvas_pixels);
                    editor.print_c_code();
                }
                if (btnClear.isClicked(window)) {
                    editor.clear();
                    canvas_texture.update(editor.canvas_pixels);
                    editor.print_c_code();
                }
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                if (event.mouseButton.x < (int)CANVAS_WIDTH && event.mouseButton.y < (int)CANVAS_HEIGHT) {
                    is_drawing = true;
                    mouse_start_x = event.mouseButton.x;
                    mouse_start_y = event.mouseButton.y;
                    mouse_curr_x = mouse_start_x;
                    mouse_curr_y = mouse_start_y;
                }
            }

            if (event.type == sf::Event::MouseMoved && is_drawing) {
                mouse_curr_x = std::max(0, std::min((int)CANVAS_WIDTH - 1, event.mouseMove.x));
                mouse_curr_y = std::max(0, std::min((int)CANVAS_HEIGHT - 1, event.mouseMove.y));
            }

            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left && is_drawing) {
                is_drawing = false;
                if (std::abs(mouse_start_x - mouse_curr_x) >= 2 || std::abs(mouse_start_y - mouse_curr_y) >= 2) {
                    sf::Color current_color(color_R, color_G, color_B, color_A);
                    editor.add_shape(active_tool, mouse_start_x, mouse_start_y, mouse_curr_x, mouse_curr_y, current_color);
                    canvas_texture.update(editor.canvas_pixels);
                    editor.print_c_code();
                }
            }
        }

        btnRect.box.setFillColor(active_tool == RECTANGLE ? sf::Color(180, 180, 180) : sf::Color(240, 240, 240));
        btnCircle.box.setFillColor(active_tool == CIRCLE ? sf::Color(180, 180, 180) : sf::Color(240, 240, 240));
        color_preview_box.setFillColor(sf::Color(color_R, color_G, color_B, color_A));

        textLabels[0].setString(sliderR.label + ": " + std::to_string(color_R));
        textLabels[0].setPosition(1320.f, 180.f);
        textLabels[1].setString(sliderG.label + ": " + std::to_string(color_G));
        textLabels[1].setPosition(1320.f, 230.f);
        textLabels[2].setString(sliderB.label + ": " + std::to_string(color_B));
        textLabels[2].setPosition(1320.f, 280.f);
        textLabels[3].setString(sliderA.label + ": " + std::to_string(color_A));
        textLabels[3].setPosition(1320.f, 330.f);

        sliderR.updateKnobPosition();
        sliderG.updateKnobPosition();
        sliderB.updateKnobPosition();
        sliderA.updateKnobPosition();

        window.clear(sf::Color(235, 235, 235));
        window.draw(canvas_sprite);

        if (is_drawing) {
            if (active_tool == RECTANGLE) {
                sf::RectangleShape preview_rect(sf::Vector2f(mouse_curr_x - mouse_start_x, mouse_curr_y - mouse_start_y));
                preview_rect.setPosition(mouse_start_x, mouse_start_y);
                preview_rect.setFillColor(sf::Color::Transparent);
                preview_rect.setOutlineColor(sf::Color::Black);
                preview_rect.setOutlineThickness(1.f);
                window.draw(preview_rect);
            } else if (active_tool == CIRCLE) {
                int cx = (mouse_start_x + mouse_curr_x) / 2;
                int cy = (mouse_start_y + mouse_curr_y) / 2;
                int r = (std::abs(mouse_start_x - mouse_curr_x) + std::abs(mouse_start_y - mouse_curr_y)) / 4;
                sf::CircleShape preview_circle(r);
                preview_circle.setPosition(cx - r, cy - r);
                preview_circle.setFillColor(sf::Color::Transparent);
                preview_circle.setOutlineColor(sf::Color::Black);
                preview_circle.setOutlineThickness(1.f);
                window.draw(preview_circle);
            }
        }

        window.draw(panel_title);
        window.draw(btnRect.box);   window.draw(btnRect.text);
        window.draw(btnCircle.box); window.draw(btnCircle.text);
        window.draw(color_preview_box);
        
        window.draw(sliderR.track); window.draw(sliderR.knob);
        window.draw(sliderG.track); window.draw(sliderG.knob);
        window.draw(sliderB.track); window.draw(sliderB.knob);
        window.draw(sliderA.track); window.draw(sliderA.knob);

        for(int i = 0; i < 4; ++i) window.draw(textLabels[i]);

        window.draw(btnUndo.box);   window.draw(btnUndo.text);
        window.draw(btnClear.box);  window.draw(btnClear.text);

        window.display();
    }

    return 0;
}

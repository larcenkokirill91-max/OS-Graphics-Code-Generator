#include <SFML/Graphics.hpp>
#include "vector_editor.cpp"
#include "gui_elements.hpp"

int main() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 4;

    sf::RenderWindow window(sf::VideoMode(1920, 1024), "OS Graphics Code Generator (C++ Speed Edition)", sf::Style::Default, settings);
    window.setFramerateLimit(60);

    OsVectorEditor editor;

    sf::Texture canvas_texture;
    canvas_texture.create(CANVAS_WIDTH, CANVAS_HEIGHT);
    canvas_texture.update(editor.canvas_pixels);
    sf::Sprite canvas_sprite(canvas_texture);

    Tool active_tool = RECTANGLE;
    int color_R = 0, color_G = 0, color_B = 0, color_A = 255;
    int code_scroll_offset = 0;

    bool is_drawing = false;
    int mouse_start_x = 0, mouse_start_y = 0;
    int mouse_curr_x = 0, mouse_curr_y = 0;

    sf::Font font;
    if (!font.loadFromFile("JetBrainsMono/JetBrainsMonoNerdFontMono-ExtraLightItalic.ttf")) {
        font.loadFromFile("JetBrainsMonoNerdFontMono-ExtraLightItalic.ttf");
    }

    Slider sliderR, sliderG, sliderB, sliderA, sliderT;
    sliderR.init(1320.f, 200.f, 240.f, 0, 255, &color_R, "Red (R)", sf::Color::Red);
    sliderG.init(1320.f, 250.f, 240.f, 0, 255, &color_G, "Green (G)", sf::Color::Green);
    sliderB.init(1320.f, 300.f, 240.f, 0, 255, &color_B, "Blue (B)", sf::Color::Blue);
    sliderA.init(1320.f, 350.f, 240.f, 0, 255, &color_A, "Alpha", sf::Color(100, 100, 100));
    sliderT.init(1320.f, 400.f, 240.f, 0, 100, &editor.current_thickness, "Thickness", sf::Color(150, 150, 150));

    Button btnRect, btnCircle, btnUndo, btnClear;
    btnRect.init(1320.f, 50.f, 115.f, 35.f, "Rectangle", font);
    btnCircle.init(1445.f, 50.f, 115.f, 35.f, "Circle", font);
    btnUndo.init(1320.f, 480.f, 115.f, 35.f, "Undo", font);
    btnClear.init(1445.f, 480.f, 115.f, 35.f, "Clear", font);

    Button btnCopy;
    btnCopy.init(1620.f, 50.f, 260.f, 35.f, "Copy C-Code to Clipboard", font);

    sf::RectangleShape code_panel_background(sf::Vector2f(280.f, 780.f));
    code_panel_background.setPosition(1620.f, 110.f);
    code_panel_background.setFillColor(sf::Color(30, 30, 30)); 
    code_panel_background.setOutlineColor(sf::Color(60, 60, 60));
    code_panel_background.setOutlineThickness(1.f);

    Slider sliderScroll;
    sliderScroll.init(1620.f, 920.f, 260.f, 0, 1000, &code_scroll_offset, "Scroll Code", sf::Color(100, 149, 237));

    sf::Text code_display_text;
    code_display_text.setFont(font);
    code_display_text.setCharacterSize(11);
    code_display_text.setFillColor(sf::Color(210, 210, 210)); 
    code_display_text.setPosition(1630.f, 125.f);

    sf::Text code_panel_title;
    code_panel_title.setFont(font);
    code_panel_title.setString("GENERATED C-CODE");
    code_panel_title.setCharacterSize(16);
    code_panel_title.setFillColor(sf::Color(50, 50, 50));
    code_panel_title.setPosition(1620.f, 15.f);

    sf::RectangleShape color_preview_box(sf::Vector2f(240.f, 30.f));
    color_preview_box.setPosition(1320.f, 120.f);
    color_preview_box.setOutlineColor(sf::Color::Black);
    color_preview_box.setOutlineThickness(1.f);

    sf::Text textLabels[6];
    for(int i = 0; i < 6; ++i) {
        textLabels[i].setFont(font);
        textLabels[i].setCharacterSize(13);
        textLabels[i].setFillColor(sf::Color::Black);
    }

    sf::Text panel_title;
    panel_title.setFont(font);
    panel_title.setString("TOOLS & COLOR");
    panel_title.setCharacterSize(16);
    panel_title.setFillColor(sf::Color(50, 50, 50));
    panel_title.setPosition(1320.f, 15.f);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            sliderR.handleEvent(event, window);
            sliderG.handleEvent(event, window);
            sliderB.handleEvent(event, window);
            sliderA.handleEvent(event, window);
            sliderT.handleEvent(event, window);
            sliderScroll.handleEvent(event, window);

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                if (btnRect.isClicked(window)) active_tool = RECTANGLE;
                if (btnCircle.isClicked(window)) active_tool = CIRCLE;
                if (btnUndo.isClicked(window)) {
                    editor.undo();
                    canvas_texture.update(editor.canvas_pixels);
                }
                if (btnClear.isClicked(window)) {
                    editor.clear();
                    canvas_texture.update(editor.canvas_pixels);
                }
                if (btnCopy.isClicked(window)) {
                    sf::Clipboard::setString(editor.get_code_string());
                    btnCopy.box.setFillColor(sf::Color(120, 230, 120)); 
                }
            }

            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                btnCopy.box.setFillColor(sf::Color(240, 240, 240));
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
        textLabels[4].setString(sliderT.label + ": " + std::to_string(editor.current_thickness));
        textLabels[4].setPosition(1320.f, 380.f);
        
        textLabels[5].setString(sliderScroll.label + ": " + std::to_string(code_scroll_offset));
        textLabels[5].setPosition(1620.f, 895.f);

        sliderR.updateKnobPosition();
        sliderG.updateKnobPosition();
        sliderB.updateKnobPosition();
        sliderA.updateKnobPosition();
        sliderT.updateKnobPosition();
        sliderScroll.updateKnobPosition();

        code_display_text.setString(editor.get_code_string());
        code_display_text.setPosition(1630.f, 125.f - code_scroll_offset);

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
        window.draw(sliderT.track); window.draw(sliderT.knob);
        for(int i = 0; i < 5; ++i) window.draw(textLabels[i]);
        window.draw(btnUndo.box);   window.draw(btnUndo.text);
        window.draw(btnClear.box);  window.draw(btnClear.text);

        window.draw(code_panel_title);
        window.draw(btnCopy.box);   window.draw(btnCopy.text);
        window.draw(code_panel_background);
        window.draw(code_display_text);
        
        window.draw(sliderScroll.track); window.draw(sliderScroll.knob);
        window.draw(textLabels[5]);

        window.display();
    }

    return 0;
}
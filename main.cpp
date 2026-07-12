#include <SFML/Graphics.hpp>
#include "vector_editor.cpp" // Логика движка
#include "gui_elements.hpp"  // Элементы интерфейса (Слайдеры, Скроллбары и Кнопки)

// Объявление глобальных переменных разрешения холста ОС
unsigned int CANVAS_WIDTH = 1280;
unsigned int CANVAS_HEIGHT = 1024;

// Структура для хранения доступных пресетов в стартовом меню
struct ResolutionPreset {
    std::string label;
    unsigned int w;
    unsigned int h;
};

// Функция отрисовки стартового диалогового окна выбора разрешения
bool showResolutionSelector(const sf::Font& font) {
    sf::RenderWindow selectWindow(sf::VideoMode(400, 500), "Select OS Screen Resolution", sf::Style::Titlebar | sf::Style::Close);
    selectWindow.setFramerateLimit(60);

    sf::Text title("CHOOSE OS RESOLUTION:", font, 18);
    title.setFillColor(sf::Color::Black);
    title.setPosition(40.f, 30.f);

    std::vector<ResolutionPreset> presets = {
        {"640 x 480   (VGA Classic)", 640, 480},
        {"800 x 600   (SVGA Retro)", 800, 600},
        {"1024 x 768  (XGA Standard)", 1024, 768},
        {"1280 x 720  (HD Ready)", 1280, 720},
        {"1280 x 1024 (SXGA Square)", 1280, 1024},
        {"1920 x 1080 (Full HD)", 1920, 1080}
    };

    std::vector<Button> buttons(presets.size());
    for (size_t i = 0; i < presets.size(); ++i) {
        buttons[i].init(40.f, 90.f + i * 60.f, 320.f, 45.f, presets[i].label, font);
    }

    bool selected = false;

    while (selectWindow.isOpen()) {
        sf::Event event;
        while (selectWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                selectWindow.close();
                return false; // Закрыли приложение полностью
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                for (size_t i = 0; i < buttons.size(); ++i) {
                    if (buttons[i].isClicked(selectWindow)) {
                        CANVAS_WIDTH = presets[i].w;
                        CANVAS_HEIGHT = presets[i].h;
                        selected = true;
                        selectWindow.close();
                        break;
                    }
                }
            }
        }

        selectWindow.clear(sf::Color(235, 235, 235));
        selectWindow.draw(title);
        for (auto& btn : buttons) {
            // Подсветка кнопок при наведении мыши
            sf::Vector2i mPos = sf::Mouse::getPosition(selectWindow);
            if (btn.box.getGlobalBounds().contains(mPos.x, mPos.y)) {
                btn.box.setFillColor(sf::Color(210, 210, 210));
            } else {
                btn.box.setFillColor(sf::Color(240, 240, 240));
            }
            selectWindow.draw(btn.box);
            selectWindow.draw(btn.text);
        }
        selectWindow.display();
    }
    return selected;
}

int main() {
    sf::Font font;
    if (!font.loadFromFile("JetBrainsMono/JetBrainsMonoNerdFontMono-ExtraLightItalic.ttf")) {
        font.loadFromFile("JetBrainsMonoNerdFontMono-ExtraLightItalic.ttf");
    }

    // Запускаем селектор разрешения перед открытием главного редактора
    if (!showResolutionSelector(font)) {
        return 0; 
    }

    // Включаем аппаратное сглаживание
    sf::ContextSettings settings;
    settings.antialiasingLevel = 4;

    sf::RenderWindow window(sf::VideoMode(1920, 1024), "OS Graphics Code Generator (C++ Speed Edition)", sf::Style::Default, settings);
    window.setFramerateLimit(60);

    OsVectorEditor editor;
    editor.init_canvas(); // Выделяем память под выбранное разрешение

    sf::Texture canvas_texture;
    canvas_texture.create(CANVAS_WIDTH, CANVAS_HEIGHT);
    canvas_texture.update(editor.canvas_pixels);
    sf::Sprite canvas_sprite(canvas_texture);

    // Центрируем и масштабируем холст, если он не влезает в рабочую область редактора (1280x1024)
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    if (CANVAS_WIDTH > 1280 || CANVAS_HEIGHT > 1024) {
        float ratioX = 1280.f / CANVAS_WIDTH;
        float ratioY = 1024.f / CANVAS_HEIGHT;
        float finalScale = std::min(ratioX, ratioY);
        canvas_sprite.setScale(finalScale, finalScale);
    }

    Tool active_tool = RECTANGLE;
    int color_R = 0, color_G = 0, color_B = 0, color_A = 255;
    
    int code_scroll_y = 0;
    int code_scroll_x = 0;

    bool is_drawing = false;
    int mouse_start_x = 0, mouse_start_y = 0;
    int mouse_curr_x = 0, mouse_curr_y = 0;

    // Инициализация элементов левой панели управления
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

    // Кнопка копирования
    Button btnCopy;
    btnCopy.init(1620.f, 50.f, 260.f, 35.f, "Copy C-Code to Clipboard", font);

    // Главное черное окно для кода (Ширина 260, Высота 840)
    sf::RectangleShape code_panel_background(sf::Vector2f(260.f, 840.f)); 
    code_panel_background.setPosition(1620.f, 110.f);
    code_panel_background.setFillColor(sf::Color(30, 30, 30)); 
    code_panel_background.setOutlineColor(sf::Color(60, 60, 60));
    code_panel_background.setOutlineThickness(1.f);

    // Размещаем вертикальный скроллбара слева от черного окна (X = 1605)
    ScrollBar scrollV, scrollH;
    scrollV.init(1605.f, 110.f, 840.f, true, &code_scroll_y);  
    scrollH.init(1620.f, 960.f, 260.f, false, &code_scroll_x); 

    // Настраиваем камеру-вьюпорт для отсечения вылетающего текста
    sf::View code_view;
    code_view.setSize(240.f, 820.f); 
    code_view.setViewport(sf::FloatRect(1630.f / 1920.f, 120.f / 1024.f, 240.f / 1920.f, 820.f / 1024.f));

    sf::Text code_display_text;
    code_display_text.setFont(font);
    code_display_text.setCharacterSize(11);
    code_display_text.setFillColor(sf::Color(210, 210, 210)); 
    code_display_text.setPosition(0.f, 0.f);

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

    sf::Text textLabels[5];
    for(int i = 0; i < 5; ++i) {
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
            
            scrollV.handleEvent(event, window);
            scrollH.handleEvent(event, window);

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
                // Пересчитываем мировые координаты мыши с учетом масштаба спрайта
                sf::Vector2i mPos = sf::Mouse::getPosition(window);
                float invScaleX = 1.0f / canvas_sprite.getScale().x;
                float invScaleY = 1.0f / canvas_sprite.getScale().y;
                int virtualMouseX = (int)(mPos.x * invScaleX);
                int virtualMouseY = (int)(mPos.y * invScaleY);

                if (virtualMouseX < (int)CANVAS_WIDTH && virtualMouseY < (int)CANVAS_HEIGHT) {
                    is_drawing = true;
                    mouse_start_x = virtualMouseX;
                    mouse_start_y = virtualMouseY;
                    mouse_curr_x = mouse_start_x;
                    mouse_curr_y = mouse_start_y;
                }
            }

            if (event.type == sf::Event::MouseMoved && is_drawing) {
                sf::Vector2i mPos = sf::Mouse::getPosition(window);
                float invScaleX = 1.0f / canvas_sprite.getScale().x;
                float invScaleY = 1.0f / canvas_sprite.getScale().y;
                
                mouse_curr_x = std::max(0, std::min((int)CANVAS_WIDTH - 1, (int)(mPos.x * invScaleX)));
                mouse_curr_y = std::max(0, std::min((int)CANVAS_HEIGHT - 1, (int)(mPos.y * invScaleY)));
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

        sliderR.updateKnobPosition();
        sliderG.updateKnobPosition();
        sliderB.updateKnobPosition();
        sliderA.updateKnobPosition();
        sliderT.updateKnobPosition();

        std::string full_code = editor.get_code_string();
        code_display_text.setString(full_code);
        sf::FloatRect textBounds = code_display_text.getLocalBounds();

        scrollV.updateSize((int)textBounds.height + 20, 820);
        scrollH.updateSize((int)textBounds.width + 40, 240);

        float centerX = 120.f + code_scroll_x;
        float centerY = 410.f + code_scroll_y;
        code_view.setCenter(centerX, centerY);

        window.clear(sf::Color(235, 235, 235));
        
        window.draw(canvas_sprite);

        if (is_drawing) {
            // Отрисовка временного контура с учетом масштабирования
            if (active_tool == RECTANGLE) {
                sf::RectangleShape preview_rect(sf::Vector2f((mouse_curr_x - mouse_start_x) * canvas_sprite.getScale().x, (mouse_curr_y - mouse_start_y) * canvas_sprite.getScale().y));
                preview_rect.setPosition(mouse_start_x * canvas_sprite.getScale().x, mouse_start_y * canvas_sprite.getScale().y);
                preview_rect.setFillColor(sf::Color::Transparent);
                preview_rect.setOutlineColor(sf::Color::Black);
                preview_rect.setOutlineThickness(1.f);
                window.draw(preview_rect);
            } else if (active_tool == CIRCLE) {
                int cx = (mouse_start_x + mouse_curr_x) / 2;
                int cy = (mouse_start_y + mouse_curr_y) / 2;
                int r = (std::abs(mouse_start_x - mouse_curr_x) + std::abs(mouse_start_y - mouse_curr_y)) / 4;
                sf::CircleShape preview_circle(r * canvas_sprite.getScale().x);
                preview_circle.setPosition((cx - r) * canvas_sprite.getScale().x, (cy - r) * canvas_sprite.getScale().y);
                preview_circle.setFillColor(sf::Color::Transparent);
                preview_circle.setOutlineColor(sf::Color::Black);
                preview_circle.setOutlineThickness(1.f);
                window.draw(preview_circle);
            }
        }

        sf::View default_view = window.getDefaultView();

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

        window.setView(code_view);
        window.draw(code_display_text);
        
        window.setView(default_view);

        if (scrollV.maxScrollValue > 0) {
            window.draw(scrollV.track);
            window.draw(scrollV.thumb);
        }
        if (scrollH.maxScrollValue > 0) {
            window.draw(scrollH.track);
            window.draw(scrollH.thumb);
        }

        window.display();
    }

    return 0;
}

#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <algorithm>

struct Slider {
    sf::RectangleShape track;
    sf::CircleShape knob;
    int minValue;
    int maxValue;
    int* valuePtr;
    std::string label;
    bool isDragging;

    void init(float x, float y, float width, int minV, int maxV, int* ptr, std::string lbl, sf::Color color) {
        minValue = minV;
        maxValue = maxV;
        valuePtr = ptr;
        label = lbl;
        isDragging = false;

        track.setSize(sf::Vector2f(width, 6.f));
        track.setPosition(x, y);
        track.setFillColor(sf::Color(200, 200, 200));

        knob.setRadius(8.f);
        knob.setPointCount(30); 
        knob.setOrigin(8.f, 8.f);
        knob.setFillColor(color);
        updateKnobPosition();
    }

    void updateKnobPosition() {
        float ratio = (float)(*valuePtr - minValue) / (maxValue - minValue);
        float posX = track.getPosition().x + ratio * track.getSize().x;
        float posY = track.getPosition().y + track.getSize().y / 2.f;
        knob.setPosition(posX, posY);
    }

    void handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            float hitboxSize = 30.f;
            sf::FloatRect extendedBounds(
                knob.getPosition().x - hitboxSize / 2.f,
                knob.getPosition().y - hitboxSize / 2.f,
                hitboxSize,
                hitboxSize
            );

            if (extendedBounds.contains(mousePos.x, mousePos.y) || 
                track.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                isDragging = true;
            }
        }
        if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
            isDragging = false;
        }
        if (isDragging && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            float trackX = track.getPosition().x;
            float trackW = track.getSize().x;
            float relativeX = (float)mousePos.x - trackX;
            if (relativeX < 0) relativeX = 0;
            if (relativeX > trackW) relativeX = trackW;

            float ratio = relativeX / trackW;
            *valuePtr = minValue + (int)(ratio * (maxValue - minValue));
            updateKnobPosition();
        }
    }
};

struct ScrollBar {
    sf::RectangleShape track;
    sf::RectangleShape thumb;
    bool isVertical;
    bool isDragging;
    int* valuePtr;
    int maxScrollValue;

    void init(float x, float y, float length, bool vertical, int* ptr) {
        isVertical = vertical;
        isDragging = false;
        valuePtr = ptr;
        maxScrollValue = 0;

        track.setPosition(x, y);
        track.setFillColor(sf::Color(45, 45, 45));
        
        if (isVertical) {
            track.setSize(sf::Vector2f(10.f, length));
            thumb.setSize(sf::Vector2f(10.f, 40.f));
        } else {
            track.setSize(sf::Vector2f(length, 10.f));
            thumb.setSize(sf::Vector2f(40.f, 10.f));
        }
        thumb.setFillColor(sf::Color(100, 100, 100));
        updateThumbPosition();
    }

    void updateSize(int contentSize, int viewSize) {
        if (contentSize <= viewSize) {
            maxScrollValue = 0;
            *valuePtr = 0;
            thumb.setSize(sf::Vector2f(0.f, 0.f));
            return;
        }
        maxScrollValue = contentSize - viewSize;
        
        float viewRatio = (float)viewSize / contentSize;
        float trackLen = isVertical ? track.getSize().y : track.getSize().x;
        float thumbLen = std::max(20.f, trackLen * viewRatio);

        if (isVertical) {
            thumb.setSize(sf::Vector2f(10.f, thumbLen));
        } else {
            thumb.setSize(sf::Vector2f(thumbLen, 10.f));
        }
        updateThumbPosition();
    }

    void updateThumbPosition() {
        if (maxScrollValue <= 0) return;
        float ratio = (float)(*valuePtr) / maxScrollValue;
        float trackLen = isVertical ? track.getSize().y : track.getSize().x;
        float thumbLen = isVertical ? thumb.getSize().y : thumb.getSize().x;
        float availableSpace = trackLen - thumbLen;

        if (isVertical) {
            thumb.setPosition(track.getPosition().x, track.getPosition().y + ratio * availableSpace);
        } else {
            thumb.setPosition(track.getPosition().x + ratio * availableSpace, track.getPosition().y);
        }
    }

    void handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
        if (maxScrollValue <= 0) return;
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::FloatRect hitbox = thumb.getGlobalBounds();
            hitbox.left -= 10.f; hitbox.top -= 10.f;
            hitbox.width += 20.f; hitbox.height += 20.f;

            if (hitbox.contains(mousePos.x, mousePos.y) || track.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                isDragging = true;
            }
        }
        if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
            isDragging = false;
        }
        if (isDragging && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            float trackStart = isVertical ? track.getPosition().y : track.getPosition().x;
            float trackLen = isVertical ? track.getSize().y : track.getSize().x;
            float thumbLen = isVertical ? thumb.getSize().y : thumb.getSize().x;
            float mouseCoord = isVertical ? mousePos.y : mousePos.x;

            float relativeMouse = mouseCoord - trackStart - (thumbLen / 2.f);
            float availableSpace = trackLen - thumbLen;
            if (relativeMouse < 0) relativeMouse = 0;
            if (relativeSpace(relativeMouse, availableSpace)) relativeMouse = availableSpace;

            float ratio = relativeMouse / availableSpace;
            *valuePtr = (int)(ratio * maxScrollValue);
            updateThumbPosition();
        }
    }

private:
    float relativeSpace(float val, float maxVal) { return val > maxVal ? maxVal : val; }
};

struct Button {
    sf::RectangleShape box;
    sf::Text text;

    void init(float x, float y, float w, float h, std::string label, const sf::Font& font) {
        box.setSize(sf::Vector2f(w, h));
        box.setPosition(x, y);
        box.setFillColor(sf::Color(240, 240, 240));
        box.setOutlineColor(sf::Color(150, 150, 150));
        box.setOutlineThickness(1.f);

        text.setFont(font);
        text.setString(label);
        text.setCharacterSize(14);
        text.setFillColor(sf::Color::Black);
        
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        text.setPosition(x + w / 2.0f, y + h / 2.0f);
    }

    bool isClicked(const sf::RenderWindow& window) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        return box.getGlobalBounds().contains(mousePos.x, mousePos.y) && 
               sf::Mouse::isButtonPressed(sf::Mouse::Left);
    }
};

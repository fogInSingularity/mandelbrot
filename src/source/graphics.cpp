#include "graphics.h"
#include "config.h"
#include "mandelbrot.h"

void Render(sf::RenderWindow& window, const Mandelbrot::MSet& m_set) {
    sf::Image image;
    image.create(kWindowWidth, kWindowHight, m_set.pixels);

    sf::Texture texture;
    texture.loadFromImage(image);

    sf::Sprite sprite;
    sprite.setTexture(texture);

    window.clear();
    window.draw(sprite);
    window.display();
}

void CheckWindowEvents(sf::RenderWindow& window, Mandelbrot::MSet* m_set) {
    assert(m_set != nullptr);
    
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        } else if (event.type == sf::Event::KeyPressed) {
            if (sf::Keyboard::isKeyPressed(kButtonMoveLeft)) {
                m_set->move_x -= 10.0f * m_set->scale;
            } else if (sf::Keyboard::isKeyPressed(kButtonMoveRight)) {
                m_set->move_x += 10.0f * m_set->scale;
            } else if (sf::Keyboard::isKeyPressed(kButtonMoveUp)) {
                m_set->move_y -= 10.0f * m_set->scale;
            } else if (sf::Keyboard::isKeyPressed(kButtonMoveDown)) {
                m_set->move_y += 10.0f * m_set->scale;
            } else if (sf::Keyboard::isKeyPressed(kButtonDefaultView)) {
                m_set->move_y = 0.0f;
                m_set->move_x = 0.0f;
                m_set->scale  = 1.0f;
            } else if (sf::Keyboard::isKeyPressed(kButtonZoomIn)) {
                m_set->scale /= 1.01f;
            } else if (sf::Keyboard::isKeyPressed(kButtonZoomOut)) {
                m_set->scale *= 1.01f;
            } else if (sf::Keyboard::isKeyPressed(kButtonQuit)) { 
                window.close();
            }
        }
    }
}
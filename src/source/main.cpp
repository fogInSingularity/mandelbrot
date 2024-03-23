#include "mandelbrot.h"
#include "graphics_cfg.h"

int main() {
    using MError = Mandelbrot::Error;
    MError m_error = MError::kOk;
    
    Mandelbrot::MSet m_set;
    m_error = Mandelbrot::SetUp(m_set);

    if (m_error != MError::kOk) {
        fprintf(stderr, "# Error: bad alloc\n");
        Mandelbrot::TearDown(m_set);
        return 1;
    }

    sf::RenderWindow window(sf::VideoMode(kWindowWidth, 
                                          kWindowHight), 
                            kWindowTitle,
                            sf::Style::Fullscreen);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::KeyPressed) {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                    m_set.move_x -= 10.0f;
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                    m_set.move_x += 10.0f;
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
                    m_set.move_y -= 10.0f;
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
                    m_set.move_y += 10.0f;
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num0)) {
                    m_set.move_y = 0.0f;
                    m_set.move_x = 0.0f;
                    m_set.scale  = 1.0f;
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
                    m_set.scale -= 0.05f;
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)) {
                    m_set.scale += 0.05f;
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) { 
                    window.close();
                }
            }
        }

        Mandelbrot::Compute(m_set);
        Mandelbrot::Render(window, m_set);
    }

    Mandelbrot::TearDown(m_set);

    return 0;
}

#include "mandelbrot.h"
#include "graphics_cfg.h"

int main(const int argc, const char** argv) {
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
            }
        }

        Mandelbrot::Compute(m_set);
        Mandelbrot::Render(window, m_set);
    }

    Mandelbrot::TearDown(m_set);

    return 0;
}

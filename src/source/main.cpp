#include "mandelbrot.h"
#include "graphics.h"

int main() {
    using MError = Mandelbrot::Error;
    MError m_error = MError::kOk;
    
    Mandelbrot::MSet m_set = {};
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
        CheckWindowEvents(window, m_set);
        Mandelbrot::Compute(m_set);
        Render(window, m_set);
    }

    Mandelbrot::TearDown(m_set);

    return 0;
}

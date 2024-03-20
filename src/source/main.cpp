#include "mandelbrot.h"
#include "graphics_cfg.h"

int main(const int argc, const char** argv) {
    using MError = Mandelbrot::Error;
    MError m_error = MError::kOk;
    
    Mandelbrot::PixelBuf pixel_buf;
    m_error = Mandelbrot::SetUp(pixel_buf);

    if (m_error != MError::kOk) {
        fprintf(stderr, "# Error: bad alloc\n");
        Mandelbrot::TearDown(pixel_buf);
        return 1;
    }

    sf::RenderWindow window(sf::VideoMode(kWindowWidth, 
                                          kWindowHight), 
                            kWindowTitle);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        Mandelbrot::Compute(pixel_buf);
        Mandelbrot::Render(window, pixel_buf.pixels);
    }

    Mandelbrot::TearDown(pixel_buf);

    return 0;
}

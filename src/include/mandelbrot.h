#ifndef MANDELBROT_H_
#define MANDELBROT_H_

#include <cstddef>
#include <cstdio>
#include <cassert>

#include <SFML/Config.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

namespace Mandelbrot {
    enum class Error {
        kOk       = 0,
        kBadAlloc = 1,
    };

    struct PixelBuf {
        sf::Uint8* pixels;
        size_t n_pixels;
    };

    Error SetUp(PixelBuf& pixel_buf);
    void TearDown(PixelBuf& pixel_buf);

    void Compute(PixelBuf& pixel_buf);
    void Render(sf::RenderWindow& window, const sf::Uint8* pixels);
}


#endif // MANDELBROT_H_
#include "mandelbrot.h"
#include "graphics_cfg.h"


Mandelbrot::Error Mandelbrot::SetUp(PixelBuf &pixel_buf) {
    pixel_buf.n_pixels = 4 * kWindowWidth * kWindowHight;
    pixel_buf.pixels = (sf::Uint8*)calloc(pixel_buf.n_pixels, sizeof(sf::Uint8));
    if (pixel_buf.pixels == nullptr) {
        return Error::kBadAlloc;
    }

    return Error::kOk;
}

void Mandelbrot::TearDown(PixelBuf &pixel_buf) {
    pixel_buf.n_pixels = 0;
    free(pixel_buf.pixels);
}

void Mandelbrot::Compute(PixelBuf &pixel_buf) {
    for (size_t i = 0; i < pixel_buf.n_pixels; i++) {
        pixel_buf.pixels[i] = 255;
    }
}

void Mandelbrot::Render(sf::RenderWindow& window, const sf::Uint8* pixels) {
    assert(pixels != nullptr);

    sf::Image image;
    image.create(kWindowWidth, kWindowHight, pixels);

    sf::Texture texture;
    texture.loadFromImage(image);

    sf::Sprite sprite;
    sprite.setTexture(texture);

    window.clear();
    window.draw(sprite);
    window.display();
}

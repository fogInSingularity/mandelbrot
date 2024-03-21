#include "mandelbrot.h"
#include "graphics_cfg.h"

// static ---------------------------------------------------------------------

static uint8_t CheckPixel(float real, float imag);

// global ---------------------------------------------------------------------

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
    for (int32_t y = 0; y < kWindowHight; y++) {
        for (int32_t x = 0; x < kWindowWidth; x++) {  
            float real = ((x - (int32_t)kWindowWidth / 2.0f) * 4.0f) / kWindowWidth;
            float imag = ((y - (int32_t)kWindowHight / 2.0f) * 4.0f)  / kWindowHight;
            
            uint8_t grad = CheckPixel(real, imag);

            pixel_buf.pixels[4 * (y * kWindowWidth + x)]     = grad;
            pixel_buf.pixels[4 * (y * kWindowWidth + x) + 1] = grad;
            pixel_buf.pixels[4 * (y * kWindowWidth + x) + 2] = grad;
            pixel_buf.pixels[4 * (y * kWindowWidth + x) + 3] = 255;            
        }
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

// static ---------------------------------------------------------------------

static uint8_t CheckPixel(float real, float imag) {
    const uint64_t kMaxIter = 255;
    
    float x = 0;
    float y = 0;

    uint32_t iter = 0;

    while (x * x + y * y < 4.0f && iter <= kMaxIter) {
        float xtemp = x * x - y * y + real;
        y = 2 * x * y + imag;
        x = xtemp;

        iter++;
    }

    if (iter >= kMaxIter) {
        return 255;
    } else {
        return (uint8_t)(iter % 255);
    }
}


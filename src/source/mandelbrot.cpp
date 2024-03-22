#include "mandelbrot.h"
#include "graphics_cfg.h"

// static ---------------------------------------------------------------------

static uint8_t CheckPixel(float real, float imag);

// global ---------------------------------------------------------------------

Mandelbrot::Error Mandelbrot::SetUp(MSet& m_set) {
    m_set.n_pixels = 4 * kWindowWidth * kWindowHight;
    m_set.pixels = (sf::Uint8*)calloc(m_set.n_pixels, sizeof(sf::Uint8));
    if (m_set.pixels == nullptr) {
        return Error::kBadAlloc;
    }

    m_set.move_x = 0.0f;
    m_set.move_y = 0.0f;
    m_set.scale  = 1.0f;

    return Error::kOk;
}

void Mandelbrot::TearDown(MSet& m_set) {
    m_set.move_x = 0.0f;
    m_set.move_y = 0.0f;
    m_set.scale  = 0.0f;
    m_set.n_pixels = 0;
    free(m_set.pixels);
}

void Mandelbrot::Compute(MSet& m_set) {
    for (int32_t y = 0; y < (int32_t)kWindowHight; y++) {
        for (int32_t x = 0; x < (int32_t)kWindowWidth; x++) {  
            float real = (((float)x - (float)kWindowWidth / 2.0f) * 4.0f) / ((kWindowWidth + kWindowHight) / 2);
            float imag = (((float)y - (float)kWindowHight / 2.0f) * 4.0f) / ((kWindowHight + kWindowWidth) / 2);
            
            uint8_t grad = CheckPixel(real, imag);

            m_set.pixels[4 * (y * (int32_t)kWindowWidth + x)]     = grad;
            m_set.pixels[4 * (y * (int32_t)kWindowWidth + x) + 1] = grad;
            m_set.pixels[4 * (y * (int32_t)kWindowWidth + x) + 2] = grad;
            m_set.pixels[4 * (y * (int32_t)kWindowWidth + x) + 3] = 255;            
        }
    }
}

void Mandelbrot::Render(sf::RenderWindow& window, const MSet& m_set) {
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

// static ---------------------------------------------------------------------

static uint8_t CheckPixel(float real, float imag) {
    const uint64_t kMaxIter = 1024;
    
    float x = 0.0f;
    float y = 0.0f;

    uint32_t iter = 0;

    while (x * x + y * y < 4.0f && iter <= kMaxIter) {
        float xtemp = x * x - y * y + real;
        y = 2.0f * x * y + imag;
        x = xtemp;

        iter++;
    }

    if (iter >= kMaxIter) {
        return 255;
    } else {
        return (uint8_t)(iter % 255);
    }
}

 
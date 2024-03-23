#ifndef MANDELBROT_H_
#define MANDELBROT_H_

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include <immintrin.h> // simd

#include <SFML/Config.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

// #define COMPUTE_NAIVE 1
// #define COMPUTE_ARRAY 1
#define COMPUTE_SIMD 1

namespace Mandelbrot {
    const size_t kMaxIter = 48;

    enum class Error {
        kOk       = 0,
        kBadAlloc = 1,
    };

    struct MSet {
        sf::Uint8* pixels;
        size_t n_pixels;
      
        float move_x;
        float move_y;
        float scale;    
    };

    Error SetUp(MSet& m_set);
    void TearDown(MSet& m_set);

    void Compute(MSet& m_set);
    void Render(sf::RenderWindow& window, const MSet& m_set);
}

#endif // MANDELBROT_H_
#include "mandelbrot.h"
#include "bench.h"
#include "debug_simd.h"

// static ---------------------------------------------------------------------

static void ColorPixel(Mandelbrot::MSet& m_set, uint8_t grad, int32_t pos);

[[maybe_unused]] static void ComputeNaive(Mandelbrot::MSet& m_set);
[[maybe_unused]] static uint8_t CheckPixelNaive(float real, float imag);

[[maybe_unused]] static void ComputeArray(Mandelbrot::MSet& m_set);
[[maybe_unused]] static uint32_t CheckPixelArray(float real[4], float imag[4]);

[[maybe_unused]] static void ComputeSimd(Mandelbrot::MSet& m_set);
[[maybe_unused]] static uint64_t CheckPixelSimd(__m256 real, __m256 imag);

__m256i create_mask(uint8_t mask);

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

    if (m_set.pixels != nullptr) {
        free(m_set.pixels);
    }
}

void Mandelbrot::Compute(MSet& m_set) {
    [[maybe_unused]] uint64_t start_time = GetTime();

#if defined(COMPUTE_NAIVE)
    ComputeNaive(m_set);
#elif defined(COMPUTE_ARRAY)
    ComputeArray(m_set);
#elif defined (COMPUTE_SIMD)
    ComputeSimd(m_set);
#endif

    [[maybe_unused]] uint64_t end_time = GetTime();
#if defined (LOG_TIME)    
    fprintf(stderr, "%lu\n", end_time - start_time);
#endif
}

// static ---------------------------------------------------------------------

static void ColorPixel(Mandelbrot::MSet& m_set, uint8_t grad, int32_t pos) {
    m_set.pixels[4 * pos]     = grad;
    m_set.pixels[4 * pos + 1] = grad;
    m_set.pixels[4 * pos + 2] = grad;
    m_set.pixels[4 * pos + 3] = 255;
}

static void ComputeNaive(Mandelbrot::MSet& m_set) {
    for (int32_t y = 0; y < (int32_t)kWindowHight; y++) {
        for (int32_t x = 0; x < (int32_t)kWindowWidth; x++) {  
            float real = m_set.scale * (((float)x - (float)kWindowWidth / 2.0f + m_set.move_x) * 4.0f) / ((kWindowWidth + kWindowHight) / 2);
            float imag = m_set.scale * (((float)y - (float)kWindowHight / 2.0f + m_set.move_y) * 4.0f) / ((kWindowHight + kWindowWidth) / 2);
            
            uint8_t grad = CheckPixelNaive(real, imag);
            ColorPixel(m_set, grad, y * (int32_t)kWindowWidth + x);
        }
    }
}

static uint8_t CheckPixelNaive(float real, float imag) {
    float x = 0.0f;
    float y = 0.0f;

    uint32_t iter = 0;

    while (x * x + y * y < 4.0f && iter <= Mandelbrot::kMaxIter) {
        float xtemp = x * x - y * y + real;
        y = 2.0f * x * y + imag;
        x = xtemp;

        iter++;
    }

    if (iter >= Mandelbrot::kMaxIter) {
        return 255;
    } else {
        return (uint8_t)(iter * 5 % 255);
    }
}

static void ComputeArray(Mandelbrot::MSet& m_set) {
    for (int32_t y = 0; y < (int32_t)kWindowHight; y++) {
        for (int32_t x = 0; x < (int32_t)kWindowWidth; x += 4) {
            float real[4];
            float imag[4];
            uint8_t grad[4] __attribute__((aligned(4)));

            for (int32_t i = 0; i < 4; i++) {
                real[i] = m_set.scale * (((float)(x + i) - (float)kWindowWidth / 2.0f + m_set.move_x) * 4.0f) / ((kWindowWidth + kWindowHight) / 2);
                imag[i] = m_set.scale * (((float)(y) - (float)kWindowHight / 2.0f + m_set.move_y) * 4.0f) / ((kWindowHight + kWindowWidth) / 2);
            }

            *(uint32_t*)grad = CheckPixelArray(real, imag);
            
            for (int32_t i = 0; i < 4; i++) {
                ColorPixel(m_set, grad[i], y * (int32_t)kWindowWidth + x + i);
            }
        }
    }
}

static uint32_t CheckPixelArray(float real[4], float imag[4]) {
    uint8_t grad[4] __attribute__((aligned(4)));

    float x[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float y[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    uint32_t iter[4] = {0, 0, 0, 0};

    float xtemp[4];

    for (int32_t i = 0; i < 4; i++) {
        while (x[i] * x[i] + y[i] * y[i] < 4.0f && iter[i] <= Mandelbrot::kMaxIter) {
            xtemp[i] = x[i] * x[i] - y[i] * y[i] + real[i];
            y[i] = 2.0f * x[i] * y[i] + imag[i];
            x[i] = xtemp[i];

            iter[i]++;
        }

        if (iter[i] >= Mandelbrot::kMaxIter) {
            grad[i] = 255;
        } else {
            grad[i] = (uint8_t)(iter[i] * 5 % 255);
        }
    }

    return *(uint32_t*)grad;
}

static void ComputeSimd(Mandelbrot::MSet& m_set) {
    for (int32_t y = 0; y < (int32_t)kWindowHight; y++) {
        for (int32_t x = 0; x < (int32_t)kWindowWidth; x += 8) {  
            float real_arr[8] __attribute__((aligned(32)));
            float imag_arr[8] __attribute__((aligned(32)));
            
            for (int32_t i = 0; i < 8; i++) {
                real_arr[i] = m_set.scale * (((float)(x + i) - (float)kWindowWidth / 2.0f + m_set.move_x) * 4.0f) / ((kWindowWidth + kWindowHight) / 2);
                imag_arr[i] = m_set.scale * (((float)(y    ) - (float)kWindowHight / 2.0f + m_set.move_y) * 4.0f) / ((kWindowHight + kWindowWidth) / 2);
            }

            __m256 real = _mm256_load_ps(real_arr);
            __m256 imag = _mm256_load_ps(imag_arr);

            uint64_t grad = CheckPixelSimd(real, imag);
            uint8_t grad_arr[8] __attribute__((aligned(8)));
            *(uint64_t*)grad_arr = grad;

            // uint8_t grad = CheckPixelNaive(real, imag);
            ColorPixel(m_set, grad_arr[0], y * (int32_t)kWindowWidth + x + 0);
            ColorPixel(m_set, grad_arr[1], y * (int32_t)kWindowWidth + x + 1);
            ColorPixel(m_set, grad_arr[2], y * (int32_t)kWindowWidth + x + 2);
            ColorPixel(m_set, grad_arr[3], y * (int32_t)kWindowWidth + x + 3);
            ColorPixel(m_set, grad_arr[4], y * (int32_t)kWindowWidth + x + 4);
            ColorPixel(m_set, grad_arr[5], y * (int32_t)kWindowWidth + x + 5);
            ColorPixel(m_set, grad_arr[6], y * (int32_t)kWindowWidth + x + 6);
            ColorPixel(m_set, grad_arr[7], y * (int32_t)kWindowWidth + x + 7);
        }
    }
}


__m256i create_mask(uint8_t mask) {
    __m256i vec_mask = _mm256_setzero_si256();
    vec_mask = _mm256_insert_epi32(vec_mask, (mask & (1 << 0)) == 0 ? 0 : -1, 0);
    vec_mask = _mm256_insert_epi32(vec_mask, (mask & (1 << 1)) == 0 ? 0 : -1, 1);
    vec_mask = _mm256_insert_epi32(vec_mask, (mask & (1 << 2)) == 0 ? 0 : -1, 2);
    vec_mask = _mm256_insert_epi32(vec_mask, (mask & (1 << 3)) == 0 ? 0 : -1, 3);
    vec_mask = _mm256_insert_epi32(vec_mask, (mask & (1 << 4)) == 0 ? 0 : -1, 4);
    vec_mask = _mm256_insert_epi32(vec_mask, (mask & (1 << 5)) == 0 ? 0 : -1, 5);
    vec_mask = _mm256_insert_epi32(vec_mask, (mask & (1 << 6)) == 0 ? 0 : -1, 6);
    vec_mask = _mm256_insert_epi32(vec_mask, (mask & (1 << 7)) == 0 ? 0 : -1, 7);

    return vec_mask;
}


static uint64_t CheckPixelSimd(__m256 real, __m256 imag) {
    __m256 x = _mm256_set1_ps(0.0f);
    __m256 y = _mm256_set1_ps(0.0f);

    uint32_t iter = 0;
    __m256i iter_count = _mm256_setzero_si256();

    // __m256i tmp_int;
    __m256i mask;
    __m256 tmp_flt;

    int check_rad = 0;

    while (iter <= Mandelbrot::kMaxIter) {
        check_rad = _mm256_movemask_ps( _mm256_cmp_ps(_mm256_add_ps(_mm256_mul_ps(x, x), _mm256_mul_ps(y, y)), _mm256_set1_ps(4.0f), _CMP_LT_OQ)); // я жду что он 0
        if (check_rad == 0) { break; }
        mask = create_mask((uint8_t)check_rad);

        iter_count = _mm256_blendv_epi8(iter_count, _mm256_add_epi32(iter_count, _mm256_set1_epi32(1)), mask);

        tmp_flt = _mm256_add_ps(_mm256_sub_ps(_mm256_mul_ps(x, x), _mm256_mul_ps(y, y)), real);
        y = _mm256_add_ps(_mm256_mul_ps(_mm256_set1_ps(2.0f), _mm256_mul_ps(x, y)), imag);
        x = tmp_flt;

        iter++;
    }

    uint8_t grad[8];

    int* stored_iter = (int*)&iter_count;
    _mm256_storeu_si256((__m256i*)stored_iter, iter_count);

    for (int i = 0; i < 8; i++) {
        if ((size_t)stored_iter[i] >= Mandelbrot::kMaxIter) {
            grad[i] = 255;
        } else {
            grad[i] = (uint8_t)((stored_iter[i] * 4)  % 255);
        }
    }

    return *(uint64_t*)grad;
}
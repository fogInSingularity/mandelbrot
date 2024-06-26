#include "mandelbrot.h"
#include "bench.h"
#include "config.h"
#include "debug_simd.h"

// static ---------------------------------------------------------------------

#define GROUP_SIZE 8
#define ALIGNE_YMM __attribute__((aligned(32)))

static const float kWindowAvgSide = (kWindowWidth + kWindowHight) / 2;

static void ColorPixel(Mandelbrot::MSet* m_set, uint8_t grad, int32_t pos);

[[maybe_unused]] static void ComputeNaive(Mandelbrot::MSet* m_set);
[[maybe_unused]] static uint8_t CheckPixelNaive(float real, float imag);

[[maybe_unused]] static void ComputeArray(Mandelbrot::MSet* m_set);
[[maybe_unused]] static uint64_t CheckPixelArray(float real[GROUP_SIZE], float imag[GROUP_SIZE]);

[[maybe_unused]] static void ComputeSimd(Mandelbrot::MSet* m_set);
[[maybe_unused]] static uint64_t CheckPixelSimd(__m256 real, __m256 imag);

// global ---------------------------------------------------------------------

Mandelbrot::Error Mandelbrot::SetUp(MSet* m_set) {
    assert(m_set != nullptr);
    
    m_set->n_pixels = 4 * kWindowWidth * kWindowHight;
    m_set->pixels = (sf::Uint8*)calloc(m_set->n_pixels, sizeof(sf::Uint8));
    if (m_set->pixels == nullptr) {
        return Error::kBadAlloc;
    }

    m_set->move_x = 0.0f;
    m_set->move_y = 0.0f;
    m_set->scale  = 1.0f;

    return Error::kOk;
}

void Mandelbrot::TearDown(MSet* m_set) {
    assert(m_set != nullptr);
    
    m_set->move_x = 0.0f;
    m_set->move_y = 0.0f;
    m_set->scale  = 0.0f;
    m_set->n_pixels = 0;

    if (m_set->pixels != nullptr) {
        free(m_set->pixels);
    }
}

void Mandelbrot::Compute(MSet* m_set) {
    assert(m_set != nullptr);
    
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
    fprintf(stdout, "%lu\n", end_time - start_time);
#endif
}

// static ---------------------------------------------------------------------

static void ColorPixel(Mandelbrot::MSet* m_set, uint8_t grad, int32_t pos) {
    assert(m_set != nullptr);
    
    // uint32_t color[255] = {
    //     #include "mcolor.h"
    // };

    // *(uint32_t*)&m_set.pixels[4 * pos] = color[grad % 256];

    m_set->pixels[4 * pos]     = grad;
    m_set->pixels[4 * pos + 1] = grad;
    m_set->pixels[4 * pos + 2] = grad;
    m_set->pixels[4 * pos + 3] = 255;
}

static void ComputeNaive(Mandelbrot::MSet* m_set) {
    assert(m_set != nullptr);

    for (int32_t y = 0; y < (int32_t)kWindowHight; y++) {
        float tmp_y = m_set->scale * ((float)y - (float)kWindowHight / 2.0f) + m_set->move_y;
        for (int32_t x = 0; x < (int32_t)kWindowWidth; x++) {  
            float real = ((m_set->scale * ((float)x - (float)kWindowWidth / 2.0f) + m_set->move_x) * 4.0f) 
                         / kWindowAvgSide;
            float imag = ((tmp_y) * 4.0f) / kWindowAvgSide;
            
            uint8_t grad = CheckPixelNaive(real, imag);
            ColorPixel(m_set, grad, y * (int32_t)kWindowWidth + x);
        }
    }
}

static uint8_t CheckPixelNaive(float real, float imag) {
    float x = 0.0f;
    float y = 0.0f;

    uint32_t iter = 0;

    float x_mul = 0;
    float y_mul = 0;

    while (x_mul + y_mul < 4.0f && iter <= Mandelbrot::kMaxIter) {
        float x_temp = x_mul - y_mul + real;
        y = 2.0f * x * y + imag;
        x = x_temp;

        x_mul = x * x;
        y_mul = y * y;
        iter++;
    }

    return (uint8_t)(iter % 255);
}

static void ComputeArray(Mandelbrot::MSet* m_set) {
    assert(m_set != nullptr);
    
    float real[GROUP_SIZE] ALIGNE_YMM = {0};
    float imag[GROUP_SIZE] ALIGNE_YMM = {0};
    uint8_t grad[GROUP_SIZE] ALIGNE_YMM = {0};

    for (int32_t y = 0; y < (int32_t)kWindowHight; y++) {
        float tmp_y = ((m_set->scale * ((float)y - (float)kWindowHight / 2.0f) + m_set->move_y) * 4.0f) 
                      / kWindowAvgSide;
        for (int32_t x = 0; x < (int32_t)kWindowWidth; x += GROUP_SIZE) {
            for (int32_t i = 0; i < GROUP_SIZE; i++) {
                real[i] = ((m_set->scale * ((float)(x + i) - (float)kWindowWidth / 2.0f) + m_set->move_x) * 4.0f)
                          / kWindowAvgSide;
                imag[i] = tmp_y;
            }

            *(uint64_t*)grad = CheckPixelArray(real, imag);
            
            for (int32_t i = 0; i < GROUP_SIZE; i++) {
                ColorPixel(m_set, grad[i], y * (int32_t)kWindowWidth + x + i);
            }
        }
    }
}

static uint64_t CheckPixelArray(float real[GROUP_SIZE], float imag[GROUP_SIZE]) {
    float x[GROUP_SIZE] ALIGNE_YMM = {0};
    float y[GROUP_SIZE] ALIGNE_YMM = {0};

    uint32_t iter = 0;
    uint32_t iter_count[GROUP_SIZE] ALIGNE_YMM = {0};

    float x_temp[GROUP_SIZE] ALIGNE_YMM = {0};
    float x_mul[GROUP_SIZE] ALIGNE_YMM = {0};
    float y_mul[GROUP_SIZE] ALIGNE_YMM = {0};

#define FOR_EACH_IN_GROUP for (int32_t i = 0; i < GROUP_SIZE; i++)

#if defined(__clang__)
    #pragma nounroll
#elif defined(__GNUG__)
    #pragma GCC unroll 0
#endif
    while (iter <= Mandelbrot::kMaxIter) {
        FOR_EACH_IN_GROUP x_mul[i] = x[i] * x[i];
        FOR_EACH_IN_GROUP y_mul[i] = y[i] * y[i];
        
                          int check_rad = 0;
        FOR_EACH_IN_GROUP check_rad += (x_mul[i] + y_mul[i] < 4.0f);
                          if (check_rad == 0) { break; }

        FOR_EACH_IN_GROUP iter_count[i] += (x_mul[i] + y_mul[i] < 4.0f);

        FOR_EACH_IN_GROUP x_temp[i] = x_mul[i] - y_mul[i] + real[i];
        FOR_EACH_IN_GROUP y[i] = 2.0f * x[i] * y[i] + imag[i];
        FOR_EACH_IN_GROUP x[i] = x_temp[i];
        
        iter++;
    }

    uint8_t grad[GROUP_SIZE] ALIGNE_YMM = {0};

    FOR_EACH_IN_GROUP grad[i] = (uint8_t)(iter_count[i] % 255);
#undef FOR_EACH_IN_GROUP

    return *(uint64_t*)grad;
}

static void ComputeSimd(Mandelbrot::MSet* m_set) {
    assert(m_set != nullptr);
    
    for (int32_t y = 0; y < (int32_t)kWindowHight; y++) {
        float y_temp = ((m_set->scale * ((float)y - (float)kWindowHight / 2.0f) + m_set->move_y) * 4.0f) 
                       / kWindowAvgSide;
        for (int32_t x = 0; x < (int32_t)kWindowWidth; x += 8) {  
            float x_temp = ((float)x - (float)kWindowWidth / 2.0f) + m_set->move_x / m_set->scale;

            __m256 real = _mm256_set_ps(7.0f, 6.0f, 5.0f, 4.0f, 3.0f, 2.0f, 1.0f, 0.0f);
            real = _mm256_add_ps(real, _mm256_set1_ps(x_temp));
            real = _mm256_mul_ps(real, _mm256_set1_ps((m_set->scale * 4.0f) / kWindowAvgSide));

            __m256 imag = _mm256_set1_ps(y_temp);

            uint64_t grad = CheckPixelSimd(real, imag);
            uint8_t grad_arr[8] __attribute__((aligned(8)));
            
            memcpy(grad_arr, &grad, sizeof(grad));

            for (int i = 0; i < 8; i++) {
               ColorPixel(m_set, grad_arr[i], y * (int32_t)kWindowWidth + x + i); 
            }
        }
    }
}

static uint64_t CheckPixelSimd(__m256 real, __m256 imag) {
    __m256 x = _mm256_setzero_ps();
    __m256 y = _mm256_setzero_ps();

    uint32_t iter = 0;
    int check_rad = 0;
    __m256i iter_count = _mm256_setzero_si256();

    while (iter <= Mandelbrot::kMaxIter) {
        __m256 x_mul = _mm256_mul_ps(x, x);
        __m256 y_mul = _mm256_mul_ps(y, y);
        __m256 mask = _mm256_cmp_ps(_mm256_add_ps(x_mul, y_mul), _mm256_set1_ps(4.0f), _CMP_LT_OQ);
        
        check_rad = _mm256_movemask_ps(mask); // wait to be 0
        if (check_rad == 0) { break; }

        iter_count = _mm256_blendv_epi8(iter_count, 
                                        _mm256_add_epi32(iter_count, _mm256_set1_epi32(1)), 
                                        (__m256i)mask);

        __m256  tmp_flt = _mm256_add_ps(_mm256_sub_ps(x_mul, y_mul), real);
        y = _mm256_add_ps(_mm256_mul_ps(_mm256_set1_ps(2.0f), _mm256_mul_ps(x, y)), imag);
        x = tmp_flt;

        iter++;
    }

    int* stored_iter ALIGNE_YMM = (int*)&iter_count;
    _mm256_storeu_si256((__m256i*)stored_iter, iter_count);

    uint8_t grad[8] ALIGNE_YMM = {0};
    for (int i = 0; i < 8; i++) {
        grad[i] = (uint8_t)(stored_iter[i] % 255);
    }

    return *(uint64_t*)grad;
}
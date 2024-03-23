#include "debug_simd.h"
#include <cstdint>
#include <cstdio>
#include <immintrin.h>

void PrintYValueFloat(__m256 value) {
    float buffer[8] __attribute__((aligned(32)));
    _mm256_store_ps(buffer, value);

    fprintf(stderr, "float vec: ");

    for (int i = 0; i < 8; i++) {
        fprintf(stderr, "%f ", buffer[i]);
    }

    fprintf(stderr, "\n");
}

void PrintYValueDouble(__m256d value) {
    double buffer[4] __attribute__((aligned(32)));
    _mm256_store_pd(buffer, value);

    fprintf(stderr, "double vec: ");

    for (int i = 0; i < 4; i++) {
        fprintf(stderr, "%x ", (uint32_t)buffer[i]);
    }

    fprintf(stderr, "\n");
}
void PrintYValueInt(__m256i value) {
    int* ptr = (int*)&value;
    _mm256_storeu_si256((__m256i*)ptr, value);

    fprintf(stderr, "int vec: ");

    for (int i = 0; i < 8; i++) {
        fprintf(stderr, "%x ", (uint32_t)ptr[i]);
    }

    fprintf(stderr, "\n");
}

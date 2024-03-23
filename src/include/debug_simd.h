#ifndef DEBUG_SIMD_H_
#define DEBUG_SIMD_H_

#include <immintrin.h>
#include <stdio.h>
#include <stdint.h>

void PrintYValueFloat(__m256 value);
void PrintYValueDouble(__m256d value);
void PrintYValueInt(__m256i value);

#endif // DEBUG_SIMD_H_
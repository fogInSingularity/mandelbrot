#include "bench.h"
#include <x86intrin.h>

uint64_t GetTime() {
    return __rdtsc();
}
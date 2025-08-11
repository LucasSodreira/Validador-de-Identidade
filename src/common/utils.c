#include "utils.h"
#include <stdlib.h>
#include <time.h>

// Platform-specific time function
#ifdef _WIN32
#include <windows.h>
double get_time() {
    LARGE_INTEGER t, f;
    QueryPerformanceCounter(&t);
    QueryPerformanceFrequency(&f);
    return (double)t.QuadPart / (double)f.QuadPart;
}
#else
double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}
#endif

void shuffle(id_t *array, size_t n) {
    if (n <= 1) return;
    // A semente deve ser definida uma única vez no início do programa (main).
    for (size_t i = n - 1; i > 0; i--) {
        size_t j = (size_t) (rand() % (i + 1));
        id_t temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

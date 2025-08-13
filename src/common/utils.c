#include "utils.h"
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
static CRITICAL_SECTION g_log_cs;
static int g_cs_initialized = 0;
#else
#include <pthread.h>
static pthread_mutex_t g_log_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

static FILE* g_req_file = NULL;
static FILE* g_resp_file = NULL;

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

// Funções auxiliares de logging thread-safe
static void log_lock(void) {
#ifdef _WIN32
    if (!g_cs_initialized) {
        InitializeCriticalSection(&g_log_cs);
        g_cs_initialized = 1;
    }
    EnterCriticalSection(&g_log_cs);
#else
    pthread_mutex_lock(&g_log_mutex);
#endif
}

static void log_unlock(void) {
#ifdef _WIN32
    LeaveCriticalSection(&g_log_cs);
#else
    pthread_mutex_unlock(&g_log_mutex);
#endif
}

int init_logging(const char* requests_path, const char* responses_path) {
    log_lock();
    if (!g_req_file) {
        g_req_file = fopen(requests_path ? requests_path : "request.txt", "a");
        if (g_req_file) setvbuf(g_req_file, NULL, _IOLBF, 0); // line buffered
    }
    if (!g_resp_file) {
        g_resp_file = fopen(responses_path ? responses_path : "responses.txt", "a");
        if (g_resp_file) setvbuf(g_resp_file, NULL, _IOLBF, 0);
    }
    int ok = (g_req_file && g_resp_file) ? 0 : -1;
    log_unlock();
    return ok;
}

void close_logging(void) {
    log_lock();
    if (g_req_file) { 
        fclose(g_req_file); 
        g_req_file = NULL; 
    }
    if (g_resp_file) { 
        fclose(g_resp_file); 
        g_resp_file = NULL; 
    }
    log_unlock();
#ifdef _WIN32
    if (g_cs_initialized) {
        DeleteCriticalSection(&g_log_cs);
        g_cs_initialized = 0;
    }
#endif
}

static void vlog_to(FILE** fpp, const char* default_name, const char* fmt, va_list ap) {
    char line[32768];
    int n = vsnprintf(line, sizeof(line), fmt, ap);
    if (n < 0) return;

    log_lock();
    if (!*fpp) {
        *fpp = fopen(default_name, "a");
        if (*fpp) setvbuf(*fpp, NULL, _IOLBF, 0);
    }
    if (*fpp) {
        fwrite(line, 1, (size_t)n, *fpp);
        fputc('\n', *fpp);
        fflush(*fpp);
    }
    log_unlock();
}

void log_request(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vlog_to(&g_req_file, "request.txt", fmt, ap);
    va_end(ap);
}

void log_response(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vlog_to(&g_resp_file, "responses.txt", fmt, ap);
    va_end(ap);
}

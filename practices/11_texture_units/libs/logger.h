#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

#ifdef __cplusplus
#include <cstdio>
#endif

// ────────────────────────────────────────────────
// Logger con colores ANSI 
// ────────────────────────────────────────────────

#ifdef LOG_USE_COLOR

    #define COLOR_RESET     "\033[0m"
    #define COLOR_GREEN     "\033[0;32m"
    #define COLOR_BRIGHT_GREEN "\033[1;32m"
    #define COLOR_YELLOW    "\033[0;33m"
    #define COLOR_RED       "\033[0;31m"
    #define COLOR_BRIGHT_RED   "\033[1;31m"

    #define LOG_INFO(fmt, ...)  \
        printf(COLOR_GREEN "[INFO]" COLOR_RESET "  %s:%d %s → " fmt "\n", \
               __FILE__, __LINE__, __func__, ##__VA_ARGS__)

    #define LOG_SUCCESS(fmt, ...)  \
        printf(COLOR_BRIGHT_GREEN "[SUCCESS]" COLOR_RESET "  %s:%d %s → " fmt "\n", \
               __FILE__, __LINE__, __func__, ##__VA_ARGS__)

    #define LOG_OK(fmt, ...)  \
        printf(COLOR_BRIGHT_GREEN "[OK]" COLOR_RESET "  %s:%d %s → " fmt "\n", \
               __FILE__, __LINE__, __func__, ##__VA_ARGS__)

    #define LOG_WARN(fmt, ...)  \
        printf(COLOR_YELLOW "[WARN]" COLOR_RESET "  %s:%d %s → " fmt "\n", \
               __FILE__, __LINE__, __func__, ##__VA_ARGS__)

    #define LOG_ERROR(fmt, ...) \
        fprintf(stderr, COLOR_BRIGHT_RED "[ERROR]" COLOR_RESET " %s:%d %s → " fmt "\n", \
                __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#else

    // Sin colores
    #define LOG_INFO(fmt, ...)  \
        printf("[INFO]  %s:%d %s → " fmt "\n", \
               __FILE__, __LINE__, __func__, ##__VA_ARGS__)

    #define LOG_SUCCESS(fmt, ...)  \
        printf("[SUCCESS]  %s:%d %s → " fmt "\n", \
               __FILE__, __LINE__, __func__, ##__VA_ARGS__)

    #define LOG_OK(fmt, ...)  \
        printf("[OK]  %s:%d %s → " fmt "\n", \
               __FILE__, __LINE__, __func__, ##__VA_ARGS__)

    #define LOG_WARN(fmt, ...)  \
        printf("[WARN]  %s:%d %s → " fmt "\n", \
               __FILE__, __LINE__, __func__, ##__VA_ARGS__)

    #define LOG_ERROR(fmt, ...) \
        fprintf(stderr, "[ERROR] %s:%d %s → " fmt "\n", \
                __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#endif

#endif // LOGGER_H
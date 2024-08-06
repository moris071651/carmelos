#ifndef LOGGER_H
#define LOGGER_H

#include <stdbool.h>

enum {
    LOGGER_LEVEL_DEBUG,
    LOGGER_LEVEL_INFO,
    LOGGER_LEVEL_WARN,
    LOGGER_LEVEL_ERROR,
    LOGGER_LEVEL_FATAL,
    LOGGER_LEVEL_NONE,
};

void logger_setup(void);
void logger_set_level(int level);

void logger_start_block(void);
void logger_end_block(void);

int logger_debug(const char* format, ...);
int logger_trace(const char* file, int line);
int logger_assert(const char* strexpr, bool expr, const char* format, ...)

int logger_info(const char* format, ...);
int logger_warning(const char* format, ...);
int logger_error(const char* format, ...);
int logger_fatal(const char* format, ...);

#define LOGGER_TRACE() (logger_trace(__FILE__, __LINE__))
#define LOGGER_ASSERT(expr, format, ...) (logger_assert(#expr, expr, format, ##__VA_ARGS__))

#endif // LOGGER_H
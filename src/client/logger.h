#ifndef LOGGER_H
#define LOGGER_H

enum {
    LOGGER_LEVEL_DEBUG,
    LOGGER_LEVEL_INFO,
    LOGGER_LEVEL_WARNING,
    LOGGER_LEVEL_ERROR,
    LOGGER_LEVEL_FATAL,
    LOGGER_LEVEL_NONE,
};

void logger_setup(void);
void logger_set_level(int level);

void logger_start_block(void);
void logger_end_block(void);

int logger_debug(const char* format, ...);
int logger_debug_lf(const char* file, int line, const char* format, ...);

int logger_info(const char* format, ...);
int logger_info_lf(const char* file, int line, const char* format, ...);

int logger_warning(const char* format, ...);
int logger_warning_lf(const char* file, int line, const char* format, ...);

int logger_error(const char* format, ...);
int logger_error_lf(const char* file, int line, const char* format, ...);

int logger_fatal(const char* format, ...);
int logger_fatal_lf(const char* file, int line, const char* format, ...);

#define LOGGER_DEBUG(format, ...) (logger_debug_lf(__FILE__, __LINE__, format, ##__VA_ARGS__))
#define LOGGER_INFO(format, ...) (logger_info_lf(__FILE__, __LINE__, format, ##__VA_ARGS__))
#define LOGGER_WARNING(format, ...) (logger_warning_lf(__FILE__, __LINE__, format, ##__VA_ARGS__))
#define LOGGER_ERROR(format, ...) (logger_error_lf(__FILE__, __LINE__, format, ##__VA_ARGS__))
#define LOGGER_FATAL(format, ...) (logger_fatal_lf(__FILE__, __LINE__, format, ##__VA_ARGS__))

#endif // LOGGER_H
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

void logger_set_level(int level);

void logger_start_block(void);
void logger_end_block(void);

int logger_debug(const char* format, ...);
int logger_info(const char* format, ...);
int logger_warning(const char* format, ...);
int logger_error(const char* format, ...);
int logger_fatal(const char* format, ...);

#endif // LOGGER_H
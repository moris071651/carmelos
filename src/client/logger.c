#include "logger.h"
#include "cleanup.h"
#include "config.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

#ifndef LOGGER_LOG_FILE
#define LOGGER_LOG_FILE "/var/log/client.carmelos.log"
#endif

#ifdef DEBUG
int log_level = LOGGER_LEVEL_DEBUG;
#else
int log_level = LOGGER_LEVEL_INFO;
#endif

FILE* log_file = NULL;

bool logger_inside_block = false;

static void logger_destroy(void) {
    logger_inside_block = false;
    fflush(log_file);
    fclose(log_file);
}

void logger_setup(void) {
    if (log_file) {
        return;
    }

    log_file = fopen(LOGGER_LOG_FILE, "w+");
    if (log_file == NULL) {
        exit(EXIT_FAILURE);
    }

    cleanup_add(logger_destroy);
}

void logger_set_level(int level) {
    if (level < 0 || level > LOGGER_LEVEL_NONE) {
        return;
    }

    log_level = level;
}

static void logger_print_prompt(const char* level) {
    char buffer[124];
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    strftime(buffer, 124, "%H:%M:%S %d/%m/%Y", t);

    fprintf(log_file, "LEVEL=[%s] TIME=[%s]\n", level, buffer);
}

static void logger_print_prompt_lf(const char* file, int line, const char* level) {
    char buffer[124];
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    strftime(buffer, 124, "%H:%M:%S %d/%m/%Y", t);

    fprintf(log_file, "LEVEL=[%s] TIME=[%s] FILE=[%s] LINE=[%d]\n", level, buffer, file, line);
}

static int logger_print_information(const char* format, va_list args) {
    int result = vfprintf(log_file, format, args);
    fprintf(log_file, "\n");
    return result;
}

static void logger_print_separator(void) {
    fprintf(log_file, "\n");
    fflush(log_file);
}

void logger_start_block(void) {
    logger_print_prompt("BLOCK");

    logger_inside_block = true;
}

void logger_end_block(void) {
    logger_print_separator();

    logger_inside_block = false;
}

int logger_debug(const char* format, ...) {
    if (log_level > LOGGER_LEVEL_DEBUG) {
        return 0;
    }

    if (!logger_inside_block) {
        logger_print_prompt("DEBUG");
    }
    
    va_list args;
    va_start(args, format);
    int result = logger_print_information(format, args);
    va_end(args);

    if (!logger_inside_block) {
        logger_print_separator();
    }

    return result;
}

int logger_debug_lf(const char* file, int line, const char* format, ...) {
    if (log_level > LOGGER_LEVEL_DEBUG) {
        return 0;
    }

    if (!logger_inside_block) {
        logger_print_prompt_lf(file, line, "DEBUG");
    }
    
    va_list args;
    va_start(args, format);
    int result = logger_print_information(format, args);
    va_end(args);

    if (!logger_inside_block) {
        logger_print_separator();
    }

    return result;
}

int logger_info(const char* format, ...) {
    if (log_level > LOGGER_LEVEL_INFO) {
        return 0;
    }
    
    if (!logger_inside_block) {
        logger_print_prompt("INFO");
    }

    va_list args;
    va_start(args, format);
    int result = logger_print_information(format, args);
    va_end(args);

    if (!logger_inside_block) {
        logger_print_separator();
    }

    return result;
}

int logger_info_lf(const char* file, int line, const char* format, ...) {
    if (log_level > LOGGER_LEVEL_INFO) {
        return 0;
    }

    if (!logger_inside_block) {
        logger_print_prompt_lf(file, line, "INFO");
    }
    
    va_list args;
    va_start(args, format);
    int result = logger_print_information(format, args);
    va_end(args);

    if (!logger_inside_block) {
        logger_print_separator();
    }

    return result;
}

int logger_warning(const char* format, ...) {
    if (log_level > LOGGER_LEVEL_WARNING) {
        return 0;
    }

    if (!logger_inside_block) {
        logger_print_prompt("WARNING");
    }

    va_list args;
    va_start(args, format);
    int result = logger_print_information(format, args);
    va_end(args);

    if (!logger_inside_block) {
        logger_print_separator();
    }

    return result;
}

int logger_warning_lf(const char* file, int line, const char* format, ...) {
    if (log_level > LOGGER_LEVEL_WARNING) {
        return 0;
    }

    if (!logger_inside_block) {
        logger_print_prompt_lf(file, line, "WARNING");
    }
    
    va_list args;
    va_start(args, format);
    int result = logger_print_information(format, args);
    va_end(args);

    if (!logger_inside_block) {
        logger_print_separator();
    }

    return result;
}

int logger_error(const char* format, ...) {
    if (log_level > LOGGER_LEVEL_ERROR) {
        return 0;
    }

    if (!logger_inside_block) {
        logger_print_prompt("ERROR");
    }

    va_list args;
    va_start(args, format);
    int result = logger_print_information(format, args);
    va_end(args);

    if (!logger_inside_block) {
        logger_print_separator();
    }

    return result;
}

int logger_error_lf(const char* file, int line, const char* format, ...) {
    if (log_level > LOGGER_LEVEL_ERROR) {
        return 0;
    }

    if (!logger_inside_block) {
        logger_print_prompt_lf(file, line, "ERROR");
    }
    
    va_list args;
    va_start(args, format);
    int result = logger_print_information(format, args);
    va_end(args);

    if (!logger_inside_block) {
        logger_print_separator();
    }

    return result;
}

int logger_fatal(const char* format, ...) {
    if (log_level > LOGGER_LEVEL_FATAL) {
        return 0;
    }

    if (!logger_inside_block) {
        logger_print_prompt("FATAL");
    }

    va_list args;
    va_start(args, format);
    int result = logger_print_information(format, args);
    va_end(args);

    if (!logger_inside_block) {
        logger_print_separator();
    }

    return result;
}

int logger_fatal_lf(const char* file, int line, const char* format, ...) {
    if (log_level > LOGGER_LEVEL_FATAL) {
        return 0;
    }

    if (!logger_inside_block) {
        logger_print_prompt_lf(file, line, "FATAL");
    }
    
    va_list args;
    va_start(args, format);
    int result = logger_print_information(format, args);
    va_end(args);

    if (!logger_inside_block) {
        logger_print_separator();
    }

    return result;
}

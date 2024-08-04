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

#ifdef NDEBUG
int log_level = LOGGER_LEVEL_INFO;
#else
int log_level = LOGGER_LEVEL_DEBUG;
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
    if (logger_inside_block) {
        return;
    }

    char buffer[124];
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    strftime(buffer, 124, "%H:%M:%S %d/%m/%Y", t);

    fprintf(log_file, "LEVEL=[%s] TIME=[%s]\n", level, buffer);
}

static int logger_print_information(const char* format, va_list args) {
    int result = vfprintf(log_file, format, args);
    fprintf(log_file, "\n");
    return result;
}

static void logger_print_separator(void) {
    if (logger_inside_block) {
        return;
    }

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

    logger_print_prompt("DEBUG");
    
    va_list args;
    va_start(args, format);
    int result = logger_print_information(format, args);
    va_end(args);

    logger_print_separator();

    return result;
}

int logger_trace(const char* file, int line) {
    if (log_level > LOGGER_LEVEL_DEBUG) {
        return 0;
    }

    logger_print_prompt("TRACE");
    
    int result = fprintf(log_file, "File: '%s', Line: %d\n", file, line);

    logger_print_separator();

    return result;
}

int logger_assert(bool expr, const char* format, ...) {
    if (log_level > LOGGER_LEVEL_DEBUG) {
        return 0;
    }

    if (expr == true) {
        return 0;
    }

#ifdef NDEBUG
    return 0;

#else
    logger_print_prompt("ASSERT");
    
    va_list args;
    va_start(args, format);
    int result = logger_print_information(format, args);
    va_end(args);

    logger_print_separator();

    abort();

    return result;
#endif
}

int logger_info(const char* format, ...) {
    if (log_level > LOGGER_LEVEL_INFO) {
        return 0;
    }
    
    logger_print_prompt("INFO");

    va_list args;
    va_start(args, format);
    int result = logger_print_information(format, args);
    va_end(args);

    logger_print_separator();

    return result;
}

int logger_warning(const char* format, ...) {
    if (log_level > LOGGER_LEVEL_WARN) {
        return 0;
    }

    logger_print_prompt("WARN");

    va_list args;
    va_start(args, format);
    int result = logger_print_information(format, args);
    va_end(args);

    logger_print_separator();

    return result;
}

int logger_error(const char* format, ...) {
    if (log_level > LOGGER_LEVEL_ERROR) {
        return 0;
    }

    logger_print_prompt("ERROR");

    va_list args;
    va_start(args, format);
    int result = logger_print_information(format, args);
    va_end(args);

    logger_print_separator();

    return result;
}

int logger_fatal(const char* format, ...) {
    if (log_level > LOGGER_LEVEL_FATAL) {
        return 0;
    }

    logger_print_prompt("FATAL");

    va_list args;
    va_start(args, format);
    int result = logger_print_information(format, args);
    va_end(args);

    logger_print_separator();

    exit(EXIT_FAILURE);
}

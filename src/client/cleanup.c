#include "cleanup.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

size_t cleanup_task_list_size = 0;
cleanup_task_t* cleanup_task_list = NULL;

void cleanup_add(cleanup_task_t task) {
    if (task == NULL) {
        return;
    }

    for (size_t i = 0; i < cleanup_task_list_size; i++) {
        if (cleanup_task_list[i] == task) {
            return;
        }
    }

    cleanup_task_list = realloc(cleanup_task_list, sizeof(cleanup_task_t) * (cleanup_task_list_size + 1));
    if (cleanup_task_list == NULL) {
        exit(EXIT_FAILURE);
    }

    cleanup_task_list[cleanup_task_list_size++] = task;
}

void cleanup_remove(cleanup_task_t task) {
    for (size_t i = 0; i < cleanup_task_list_size; i++) {
        if (cleanup_task_list[i] == task) {
            if (cleanup_task_list_size <= 1) {
                free(cleanup_task_list);

                cleanup_task_list = NULL;
                cleanup_task_list_size = 0;

                break;
            }

            if (cleanup_task_list_size - i - 1 != 0) {
                memmove(&cleanup_task_list[i], &cleanup_task_list[i + 1],
                    sizeof(cleanup_task_t) * (cleanup_task_list_size - i - 1));
            }

            cleanup_task_list_size--;
            cleanup_task_list = realloc(cleanup_task_list, sizeof(cleanup_task_t) * cleanup_task_list_size);
            if (cleanup_task_list == NULL) {
                exit(EXIT_FAILURE);
            }

            break;
        }
    }
}

void cleanup_execute(void) {
    for (size_t i = 0; i < cleanup_task_list_size; i++) {
        if (cleanup_task_list[i]) {
            cleanup_task_list[i]();
        }
    }

    free(cleanup_task_list);

    cleanup_task_list = NULL;
    cleanup_task_list_size = 0;
}

static void cleanup_SIGABRT_handler(int signum) {
    if (signum != SIGABRT) {
        exit(EXIT_FAILURE);
    }

    cleanup_execute();

    signal(signum, SIG_DFL);
    kill(getpid(), signum);
}

static void cleanup_SIGFPE_handler(int signum) {
    if (signum != SIGFPE) {
        exit(EXIT_FAILURE);
    }

    cleanup_execute();

    signal(signum, SIG_DFL);
    kill(getpid(), signum);
}

static void cleanup_SIGILL_handler(int signum) {
    if (signum != SIGILL) {
        exit(EXIT_FAILURE);
    }

    cleanup_execute();

    signal(signum, SIG_DFL);
    kill(getpid(), signum);
}

static void cleanup_SIGINT_handler(int signum) {
    if (signum != SIGINT) {
        exit(EXIT_FAILURE);
    }

    cleanup_execute();

    signal(signum, SIG_DFL);
    kill(getpid(), signum);
}

static void cleanup_SIGSEGV_handler(int signum) {
    if (signum != SIGSEGV) {
        exit(EXIT_FAILURE);
    }

    cleanup_execute();

    signal(signum, SIG_DFL);
    kill(getpid(), signum);
}

static void cleanup_SIGTERM_handler(int signum) {
    if (signum != SIGTERM) {
        exit(EXIT_FAILURE);
    }

    cleanup_execute();

    signal(signum, SIG_DFL);
    kill(getpid(), signum);
}

void cleanup_setup(void) {
    signal(SIGABRT, cleanup_SIGABRT_handler);
    signal(SIGFPE, cleanup_SIGFPE_handler);
    signal(SIGILL, cleanup_SIGILL_handler);
    signal(SIGINT, cleanup_SIGINT_handler);
    signal(SIGSEGV, cleanup_SIGSEGV_handler);
    signal(SIGTERM, cleanup_SIGTERM_handler);

    atexit(cleanup_execute);
}

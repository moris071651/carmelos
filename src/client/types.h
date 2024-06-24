#ifndef TYPES_H
#define TYPES_H

#include <time.h>

typedef struct {
    char name[128];
    char passwd[128];
    char token[256];
} user_t;

typedef struct {
    char id[512];
    time_t date;
    char name[128];
} tree_item_t;

typedef struct {
    char id[512];
    time_t date;
    char* content;
    char name[128];
} editor_item_t;

#endif // TYPES_H
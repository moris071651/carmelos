#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "config.h"

#ifndef FUZZY_SEARCH_MAX_DISTANCE
#define FUZZY_SEARCH_MAX_DISTANCE 2
#endif

static int min(int a, int b, int c) {
    if (a < b) {
        return a < c ? a : c;
    }
    else {
        return b < c ? b : c;
    }
}

static int fuzzy_distance(const char* str1, const char* str2) {
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str1);

    int** distance = malloc((len1 + 1) * sizeof(int*));
    for (size_t i = 0; i <= len1; i++) {
        distance[i] = malloc((len2 + 1) * sizeof(int));
    }

    for (size_t i = 0; i <= len1; i++) {
        distance[i][0] = i;
    }
    for (size_t i = 0; i <= len2; i++) {
        distance[0][i] = i;
    }

    for (size_t i = 1; i <= len1; i++) {
        for (size_t j = 1; j <= len2; j++) {
            int cost = (str1[i - 1] == str2[j - 1]) ? 0 : 1;
            distance[i][j] = min(distance[i - 1][j] + 1, distance[i][j - 1] + 1, distance[i - 1][j - 1] + cost);
        }
    }

    int ret = distance[len1][len2];

    for (size_t i = 0; i <= len1; i++) {
        free(distance[i]);
    }
    free(distance);

    return ret;
}

bool fuzzy_search(const char* query, const char* target) {
    size_t target_len = strlen(target);
    size_t query_len = strlen(query);

    for (size_t i = 0; i <= target_len - query_len; i++) {
        char substring[query_len + 1];
        strncpy(substring, target + i, query_len);
        substring[query_len] = '\0';

        int distance = fuzzy_distance(query, substring);
        if (distance <= FUZZY_SEARCH_MAX_DISTANCE) {
            return true;
        }
    }

    return false;
}

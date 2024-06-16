#ifndef FILESTORAGE_H
#define FILESTORAGE_H

#include <time.h>
#define FILE_STORAGE_PATH "./files"

typedef struct {
    char *username;
    time_t timestamp;
    char *filename;
    size_t size;
    char *content;
} File;

typedef struct {
    char *id;
    time_t timestamp;
    char *filename;
} FileMeta;

typedef struct {
    size_t size;
    char *content;
} FileContent;

void initFileStorage();

void saveFile(File *file);

void getFile(char *id, FileContent *file);

void deleteFile(char *id);

void listFiles(char *username, FileMeta **files, int *count);

#endif
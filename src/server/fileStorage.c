#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "fileStorage.h"
#include "CRC.h"

// small helper function to use later

static void get_file_path(char *path, char *id) {
    sprintf(path, FILE_STORAGE_PATH "/%s", id);
}

static void hash_username(char *username, char *hash) {
    CRC crc;
    CRC_Init(&crc);
    CRC_Update(&crc, username, strlen(username));
    unsigned int crc32 = CRC_Final(&crc);
    sprintf(hash, "%u", crc32);
}


static void extract_id(char *id, FileMeta *meta) {
    meta->id = malloc(strlen(id) + 1);
    strcpy(meta->id, id);
    char *tmp = malloc(strlen(id) + 1);
    strcpy(tmp, id);
    char *token = strtok(tmp, "-");
    token = strtok(NULL, "-");
    meta->timestamp = atol(token);
    token = strtok(NULL, "-");
    meta->filename = malloc(strlen(token) + 1);
    strcpy(meta->filename, token);
}


// Implement the functions declared in fileStorage.h

void initFileStorage() {
    system("mkdir -p " FILE_STORAGE_PATH);
}

void generate_id(File *file, char *id) {
    unsigned char hash[32];
    hash_username(file->username, hash);
    sprintf(id, "%s-%ld-%s", hash, file->timestamp, file->filename);
}

void saveFile(File *file) {
    // size = 32 + 1 + 20 + 1 + 256 + 1 = 311
    char id[311];
    generate_id(file, id);
    char path[256];
    get_file_path(path, id);
    FILE *f = fopen(path, "w");
    if (f == NULL) {
        printf("Failed to open file\n");
        return;
    }
    fwrite(file->content, 1, file->size, f);
    fclose(f);
}

void getFile(char *id, FileContent *file) {
    char path[256];
    get_file_path(path, id);
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        return;
    }
    fseek(f, 0, SEEK_END);
    file->size = ftell(f);
    fseek(f, 0, SEEK_SET);
    file->content = malloc(file->size + 1);
    fread(file->content, 1, file->size, f);
    fclose(f);
    file->content[file->size] = '\0';
}

void deleteFile(char *id) {
    char path[256];
    get_file_path(path, id);
    remove(path);
}

void listFiles(char *username, FileMeta **files, int *count) {
    char hash[32];
    hash_username(username, hash);
    printf("hash: %s\n", hash);
    DIR *dir = opendir(FILE_STORAGE_PATH);
    if (dir == NULL) {
        return;
    }
    struct dirent *entry;
    *count = 0;
    while ((entry = readdir(dir)) != NULL) {

        if ((strncmp(entry->d_name, hash, strlen(hash))) == 0) {
            *count += 1;
            *files = realloc(*files, *count * sizeof(FileMeta));
            printf("file: %s\n", entry->d_name);
            extract_id(entry->d_name, *files + *count - 1);
        }
    }
    closedir(dir);
}


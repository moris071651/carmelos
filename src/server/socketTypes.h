#ifndef SOCKETTYPES_H
#define SOCKETTYPES_H

#include <stdbool.h>
#include <time.h>



typedef struct {
    int type;
    char username[256];
    char password[256];
} User;

typedef struct {
    int type;
    char id[512];
    char filename[256];
    time_t timestamp;
    size_t size;
} FileSocket;

typedef struct {
    int type;
    char id[512];
    char filename[256];
    time_t timestamp;
} FileMeta_Socket;

typedef struct {
    int type;
    char username[256];
} userResponse;

typedef union {
    int type;
    User login;
    User signup;
    userResponse response;
    FileSocket newItem;
    FileMeta_Socket newItem_response;
    FileMeta_Socket delItem;
    FileMeta_Socket delItem_response;
    FileMeta_Socket getItem;
    FileSocket getItem_response;
    FileSocket updateItem;
    FileMeta_Socket updateItem_response;
} AllData;

#endif
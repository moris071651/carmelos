#ifndef SOCKETTYPES_H
#define SOCKETTYPES_H

#include <stdbool.h>
#include <time.h>

enum {
    LOGIN_TYPE,
    SIGNUP_TYPE,
    RESPONSE_TYPE,
    NEWITEM_TYPE,
    NEWITEM_RESPONSE_TYPE,
    DELITEM_TYPE,
    DELITEM_RESPONSE_TYPE,
    GETITEM_TYPE,
    GETITEM_RESPONSE_TYPE,
    UPDATEITEM_TYPE,
    UPDATEITEM_RESPONSE_TYPE,
};

typedef struct {
    int type;
    char username[256];
    char password[256];
} User;

typedef struct {
    int type;
    char id[512];
    char filename[256];
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
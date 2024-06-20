#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sqlite.h"
#include "AES-PCBC.h"
#include "fileStorage.h"
#include "CRC.h"
#include "socket_work.h"


void encryptFileContent(File *file, AES_PCBC *aes_pcbc) {
    size_t size = file->size % 16 == 0 ? file->size : file->size + (16 - file->size % 16);
    char content_tmp[size + 1];
    strcpy(content_tmp, file->content);
    content_tmp[file->size] = '\0';
    AES_PCBC_Encrypt(aes_pcbc, content_tmp, file->size);
    file->content = realloc(file->content, size);
    strcpy(file->content, content_tmp);
    file->size = size;
}

void decryptFileContent(FileContent *content, AES_PCBC *aes_pcbc) {
    content->size = content->size % 16 == 0 ? content->size : content->size + (16 - content->size % 16);
    char content_tmp[content->size + 1];
    strcpy(content_tmp, content->content);
    content_tmp[content->size] = '\0';
    AES_PCBC_Decrypt(aes_pcbc, content_tmp, content->size);
    content->content = realloc(content->content, content->size);
    strcpy(content->content, content_tmp);
    // file->size = strlen(file->content);
}

void hashPassword(char *password, char *hash) {
    CRC crc;
    CRC_Init(&crc);
    CRC_Update(&crc, password, strlen(password));
    unsigned int crc32 = CRC_Final(&crc);
    sprintf(hash, "%u", crc32);
}

void initAll(SQLite *sqlite) {
    SQLite_Open(sqlite, "test.db");
    SQLite_Execute(sqlite, "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY AUTOINCREMENT, username TEXT, password TEXT)", NULL, NULL);
    initFileStorage();
}

void signUp(SQLite *sqlite, char *username, char *password) {
    char hash[32];
    hashPassword(password, hash);
    SQLite_UserInsert(sqlite, username, hash);
}

void login(SQLite *sqlite, char *username, char *password, bool *success) {
    char hash[32];
    hashPassword(password, hash);
    bool correct;
    SQLite_UserCheck(sqlite, username, hash, &correct);
    *success = correct;
}

void Call_Functions(SQLite *sqlite, Socket *sock, char *title, AES_PCBC *aes_pcbc) {
    // check every possible call and call full_function
    if(strcmp(title, "signUp") == 0) {
        full_signUp(sqlite, sock, aes_pcbc);
    }
    else if(strcmp(title, "login") == 0) {
        full_login(sqlite, sock, aes_pcbc);
    }
    else if(aes_pcbc->key[0] == '\0' || aes_pcbc->iv[0] == '\0') {
        Response response;
        response.success = false;
        strcpy(response.message, "You need to login first");
        Socket_SendResponse(sock, &response);
    }
    else {
        Response response;
        response.success = false;
        strcpy(response.message, "Invalid title");
        Socket_SendResponse(sock, &response);
    }
}

// main

int main(int argc, char *argv[]) {
    SQLite sqlite;
    initAll(&sqlite);

    //wait for connection
    Socket sock;
    Socket_Init(&sock);
    Socket_Open(&sock, 1337);
    while(1){
        if(sock.connected){
            break;
        }
    }
    AES_PCBC aes_pcbc;
    AES_PCBC_Init(&aes_pcbc);
    while(1){
        Socket_Wait(&sock);
    }
}
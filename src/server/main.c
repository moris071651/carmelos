#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sqlite.h"
#include "AES-PCBC.h"
#include "fileStorage.h"
#include "CRC.h"
#include "socket_work.h"

void full_listItems(AllData *data, Socket *sock, char *username);

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

void hashID(char *id, char *hash) {
    CRC crc;
    CRC_Init(&crc);
    CRC_Update(&crc, id, strlen(id));
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

// full functions

void full_signUp(AllData *data, SQLite *sqlite, Socket *sock, AES_PCBC *aes_pcbc, AES_PCBC_Data *key) {
    char username[256];
    char password[256];
    strcpy(username, data->signup.username);
    strcpy(password, data->signup.password);
    hashPassword(password, password);
    bool success = 0;
    login(sqlite, username, password, &success);
    if (success) {
        userResponse response;
        response.type = 3;
        response.username[0] = '\0';
        Socket_SendResponse(sock, &response);
    } else {
        signUp(sqlite, username, password);
        userResponse response;
        response.type = 3;
        strcpy(response.username, username);
        Socket_SendResponse(sock, &response);
        strcpy(key->data, data->signup.password);
        key->data_len = strlen(data->signup.password);
        AES_PCBC_Setup(aes_pcbc, key, key, 1);

        full_listItems(data, sock, username);
    }
}

void full_login(AllData *data, SQLite *sqlite, Socket *sock, AES_PCBC *aes_pcbc, AES_PCBC_Data *key) {
    char username[256];
    char password[256];
    strcpy(username, data->login.username);
    strcpy(password, data->login.password);
    hashPassword(password, password);
    bool success = 0;
    login(sqlite, username, password, &success);
    if (success) {
        userResponse response;
        response.type = 3;
        strcpy(response.username, username);
        Socket_SendResponse(sock, &response);
        strcpy(key->data, data->login.password);
        key->data_len = strlen(data->login.password);
        AES_PCBC_Setup(aes_pcbc, key, key, 1);

        full_listItems(data, sock, username);
    } else {
        userResponse response;
        response.type = 3;
        response.username[0] = '\0';
        Socket_SendResponse(sock, &response);
    }
}

void full_newItem(AllData *data, Socket *sock, AES_PCBC *aes_pcbc, AES_PCBC_Data *key) {
    File file;
    strcpy(file.filename, data->newItem.filename);
    time_t timestamp;
    time(&timestamp);
    file.timestamp = timestamp;
    file.size = data->newItem.size;
    if (file.size > 0) {
        file.content = malloc(file.size);
        Socket_ReceiveContent(sock, file.content, file.size);
        int count = 0;
        char count_str[16];
        char tmp_id[512];
        strcpy(tmp_id, data->newItem.filename);
        char tmp_timestamp[32];
        sprintf(tmp_timestamp, "%ld", file.timestamp);
        strcat(tmp_id, tmp_timestamp);
        hashID(tmp_id, count_str);
        count = atoi(count_str);
        AES_PCBC_Setup(aes_pcbc, key, key, count);
        encryptFileContent(&file, aes_pcbc);        
    } else {
        file.content = malloc(1);
        file.content[0] = '\0';
    }
    saveFile(&file);

    FileMeta_Socket meta;
    meta.type = 5;

    generate_id(&file, meta.id);
    strcpy(meta.filename, file.filename);
    meta.timestamp = file.timestamp;
    Socket_SendFileMeta(sock, &meta, 5);
}

void full_deleteItem(AllData *data, Socket *sock) {
    FileMeta_Socket meta;
    meta = data->delItem;
    deleteFile(meta.id);
    meta.type = 7;
    Socket_SendFileMeta(sock, &meta, 7);
}

void full_getItem(AllData *data, Socket *sock, AES_PCBC *aes_pcbc, AES_PCBC_Data *key) {
    FileContent content;
    FileMeta_Socket meta;
    meta = data->getItem;
    getFile(meta.id, &content);
    //set up the aes
    int count = 0;
    char count_str[16];
    char tmp_id[512];
    strcpy(tmp_id, meta.filename);
    char tmp_timestamp[32];
    sprintf(tmp_timestamp, "%ld", meta.timestamp);
    strcat(tmp_id, tmp_timestamp);
    hashID(tmp_id, count_str);
    count = atoi(count_str);
    AES_PCBC_Setup(aes_pcbc, key, key, count);
    //end of aes setup
    decryptFileContent(&content, aes_pcbc);
    FileSocket file;
    file.type = 9;
    strcpy(file.id, meta.id);
    strcpy(file.filename, meta.filename);
    file.timestamp = meta.timestamp;
    file.size = content.size;
    Socket_SendFile(sock, &file, content.content);
}

void full_updateItem(AllData *data, Socket *sock, AES_PCBC *aes_pcbc, AES_PCBC_Data *key) {
    File file;
    strcpy(file.filename, data->updateItem.filename);
    time_t timestamp;
    time(&timestamp);
    file.timestamp = timestamp;
    file.size = data->updateItem.size;
    if (file.size > 0) {
        file.content = malloc(file.size);
        Socket_ReceiveContent(sock, file.content, file.size);
        int count = 0;
        char count_str[16];
        char tmp_id[512];
        strcpy(tmp_id, data->updateItem.filename);
        char tmp_timestamp[32];
        sprintf(tmp_timestamp, "%ld", file.timestamp);
        strcat(tmp_id, tmp_timestamp);
        hashID(tmp_id, count_str);
        count = atoi(count_str);
        AES_PCBC_Setup(aes_pcbc, key, key, count);
        encryptFileContent(&file, aes_pcbc);
    } else {
        file.content = malloc(1);
        file.content[0] = '\0';
    }
    saveFile(&file);

    FileMeta_Socket meta;
    meta.type = 11;
    generate_id(&file, meta.id);
    strcpy(meta.filename, file.filename);
    meta.timestamp = file.timestamp;
    Socket_SendFileMeta(sock, &meta, 11);
}

void full_listItems(AllData *data, Socket *sock, char *username) {
    FileMeta *files;
    int count;
    listFiles(username, &files, &count);
    FileMeta_Socket metas[count];
    for (int i = 0; i < count; i++) {
        metas[i].type = 5;
        strcpy(metas[i].id, files[i].id);
        strcpy(metas[i].filename, files[i].filename);
        metas[i].timestamp = files[i].timestamp;
    }
    Socket_SendFileMetas(sock, metas, count);
}



void Call_Functions(SQLite *sqlite, Socket *sock, int type, AES_PCBC *aes_pcbc, AES_PCBC_Data *key, AllData *data) {
    switch (type) {
        case 1:
            full_login(data, sqlite, sock, aes_pcbc, key);
            break;
        case 2:
            full_signUp(data, sqlite, sock, aes_pcbc, key);
            break;
        case 4:
            full_newItem(data, sock, aes_pcbc, key);
            break;
        case 6:
            full_deleteItem(data, sock);
            break;
        case 8:
            full_getItem(data, sock, aes_pcbc, key);
            break;
        case 10:
            full_updateItem(data, sock, aes_pcbc, key);
            break;
        default:
            break;
    }
}

// main

int main(int argc, char *argv[]) {
    SQLite sqlite;
    initAll(&sqlite);

    //wait for connection
    Socket sock;
    Socket_Init(&sock);
    Socket_Open(&sock, "socket");
    while(1){
        if(sock.connected){
            break;
        }
    }
    AES_PCBC aes_pcbc;
    AES_PCBC_Data key;
    AES_PCBC_Init(&aes_pcbc);
    while(1){
        AllData data;
        Socket_Receive(&sock, &data);
        Call_Functions(&sqlite, &sock, data.type, &aes_pcbc, &key, &data);
    }
}
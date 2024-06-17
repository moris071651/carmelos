#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sqlite.h"
#include "AES-PCBC.h"
#include "fileStorage.h"

int main(int argc, char* arv[]) {
    AES_PCBC aes_pcbc;
    AES_PCBC_Data key;
    AES_PCBC_Data iv;
    unsigned char data[] = "Hello World! Hello World!aass";
    unsigned int data_len = strlen(data);

    key.data = (unsigned char*)"1234567890123456";
    key.data_len = 16;

    iv.data = (unsigned char*)"1234567890123456";
    iv.data_len = 16;

    AES_PCBC_Setup(&aes_pcbc, &key, &iv, 0);
    AES_PCBC_Encrypt(&aes_pcbc, data, data_len);
    printf("Encrypted: %s\n", data);
    data_len = data_len % 16 == 0 ? data_len : data_len + (16 - data_len % 16);
    AES_PCBC_Decrypt(&aes_pcbc, data, data_len);
    printf("Decrypted: %s\n", data);

    // Test_parts();

    // SQLite sqlite;
    // SQLite_Open(&sqlite, "test.db");
    // SQLite_Execute(&sqlite, "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY AUTOINCREMENT, username TEXT, password TEXT)", NULL, NULL);

    // bool exists;
    // SQLite_UserExists(&sqlite, "admin", &exists);
    // if (!exists) {
    //     SQLite_UserInsert(&sqlite, "admin", "admin");
    // }

    // SQLite_Close(&sqlite);

    // test file storage

    initFileStorage();

    File file;
    file.username = "admin";
    file.timestamp = time(NULL);
    file.filename = "Test_Note";
    file.content = malloc(30);
    strcpy(file.content, "Hello World! Hello World!");
    file.size = strlen(file.content);

    //convert content to char array

    AES_PCBC_Setup(&aes_pcbc, &key, &iv, 0);


    size_t size = file.size % 16 == 0 ? file.size : file.size + (16 - file.size % 16);
    char content_tmp[size + 1];
    strcpy(content_tmp, file.content);
    content_tmp[file.size] = '\0';
    AES_PCBC_Encrypt(&aes_pcbc, content_tmp, file.size);
    file.content = realloc(file.content, size);
    strcpy(file.content, content_tmp);
    file.size = size;
    printf("Encrypted content: %s\n", file.content);

    //I should really fix this mess....
    // Ig later cause rn i'll cry if i see this again

    saveFile(&file);

    FileMeta *meta;
    int count;

    listFiles("admin", &meta, &count);

    for (int i = 0; i < count; i++) {
        printf("File: %s\n", meta[i].filename);
    }

    FileContent content;
    getFile(meta[0].id, &content);


    content.size = content.size % 16 == 0 ? content.size : content.size + (16 - content.size % 16);
    char content_tmp2[content.size + 1];
    strcpy(content_tmp2, content.content);
    content_tmp2[content.size] = '\0';
    AES_PCBC_Decrypt(&aes_pcbc, content_tmp2, content.size);
    content.content = realloc(content.content, content.size);
    strcpy(content.content, content_tmp2);

    printf("File content: %s\n", content.content);

    deleteFile(meta[0].id);
}

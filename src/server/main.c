#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "AES-PCBC.h"

int main(int argc, char* arv[]) {
    AES_PCBC aes_pcbc;
    AES_PCBC_Data key;
    AES_PCBC_Data iv;
    unsigned char data[30] = "Hello World! Hello World!aass";
    unsigned int data_len = strlen(data);

    key.data = (unsigned char*)"1234567890123456";
    key.data_len = 16;

    iv.data = (unsigned char*)"1234567890123456";
    iv.data_len = 16;

    AES_PCBC_Setup(&aes_pcbc, &key, &iv);
    AES_PCBC_Encrypt(&aes_pcbc, data, data_len);
    printf("Encrypted: %s\n", data);
    data_len = data_len % 16 == 0 ? data_len : data_len + (16 - data_len % 16);
    AES_PCBC_Decrypt(&aes_pcbc, data, data_len);
    printf("Decrypted: %s\n", data);

    // Test_parts();
}

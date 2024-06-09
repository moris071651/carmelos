#ifndef AES_PCBC_H
#define AES_PCBC_H


typedef struct {
    unsigned char key[16];
    unsigned char iv[16];
} AES_PCBC;

typedef struct {
    unsigned char *data;
    unsigned int data_len;
} AES_PCBC_Data;

static unsigned int count = 0;

void AES_PCBC_Setup(AES_PCBC *aes_pcbc, AES_PCBC_Data *key, AES_PCBC_Data *iv);

void AES_PCBC_Encrypt(AES_PCBC *aes_pcbc, unsigned char *data, unsigned int data_len);

void AES_PCBC_Decrypt(AES_PCBC *aes_pcbc, unsigned char *data, unsigned int data_len);

#endif
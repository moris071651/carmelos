#ifndef AES_PCBC_H
#define AES_PCBC_H


typedef struct {
    unsigned char key[17];
    unsigned char iv[17];
} AES_PCBC;

typedef struct {
    unsigned char *data;
    unsigned int data_len;
} AES_PCBC_Data;

void AES_PCBC_Init(AES_PCBC *aes_pcbc);

void AES_PCBC_Setup(AES_PCBC *aes_pcbc, AES_PCBC_Data *key, AES_PCBC_Data *iv, unsigned int num);

void AES_PCBC_Encrypt(AES_PCBC *aes_pcbc, unsigned char *data, unsigned int data_len);

void AES_PCBC_Decrypt(AES_PCBC *aes_pcbc, unsigned char *data, unsigned int data_len);

void Test_parts();

#endif
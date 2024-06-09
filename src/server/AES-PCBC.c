// this will be an aes implementation using PCBC mode
// this will be used to encrypt and decrypt the data

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "AES-PCBC.h"


// first: simple small function that will be used commonly in the encryption and decryption functions
// we don't need to expose theses functions to the user, so we will make them static

static void xor_block(unsigned char *block1, unsigned char *block2) {
    for (int i = 0; i < 16; i++) {
        block1[i] ^= block2[i];
    }
}

static void generate_matrix(unsigned char *data, unsigned char data_matrix[4][4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            data_matrix[j][i] = data[i * 4 + j];
        }
    }
}

static void pad_data(unsigned char *data, unsigned int data_len) {
    unsigned int padding_len = data_len % 16 == 0 ? 0 : 16 - (data_len % 16);
    unsigned char padding = (unsigned char)padding_len;
    for (int i = 0; i < padding_len; i++) {
        data[data_len + i] = padding;
    }
}

//key operations and expansions

static void RotWord(unsigned char *word) {
    unsigned char temp = word[0];
    word[0] = word[1];
    word[1] = word[2];
    word[2] = word[3];
    word[3] = temp;
}

static void SubWord(unsigned char *word, unsigned char *sbox) {
    for (int i = 0; i < 4; i++) {
        word[i] = sbox[word[i]];
    }
}

static void Rcon(unsigned char *word, int i) {
    word[0] ^= (unsigned char)(pow(2, i - 1));
}

static void KeyExpansion(unsigned char *key, unsigned char *expanded_key, unsigned char *sbox) {
    unsigned char temp[4];
    for (int i = 0; i < 16; i++) {
        expanded_key[i] = key[i];
    }
    for (int i = 4; i < 44; i++) {
        for (int j = 0; j < 4; j++) {
            temp[j] = expanded_key[(i - 1) * 4 + j];
        }
        if (i % 4 == 0) {
            RotWord(temp);
            SubWord(temp, sbox);
            Rcon(temp, i / 4);
        }
        for (int j = 0; j < 4; j++) {
            expanded_key[i * 4 + j] = expanded_key[(i - 4) * 4 + j] ^ temp[j];
        }
    }
}

//convert key from random data to a key that can be used in the encryption and decryption functions - 16 bytes
// create optional value for counter to make the IV unique
static void convert_to_16b(unsigned char *key, size_t key_len, unsigned int counter) {
    unsigned int key_int = 0;
    for (int i = 0; i < key_len; i++) {
        key_int *= (key[i] / 10) % 10 == 0 ? (key[i]/100)%10 == 0 ? 10 : 1000 : (key[i] / 100) % 10 == 0 ? 100 : 1000;
        key_int += key[i];
    }
    srand(key_int);
    counter += 1;
    for(int c = 0; c < counter; c++) {
        for (int i = 0; i < 16; i++) {
            key[i] = (unsigned char)(rand() % 256);
        }
    }

}

static void generate_sbox(unsigned char *sbox, unsigned char *key) {
    for (int i = 0; i < 256; i++) {
        sbox[i] = (unsigned char)i;
    }
    unsigned char j = 0;
    for (int i = 0; i < 256; i++) {
        j = (unsigned char)(j + sbox[i] + key[i % 16]);
        unsigned char temp = sbox[i];
        sbox[i] = sbox[j];
        sbox[j] = temp;
    }
}

static void shift_rows(unsigned char data_matrix[4][4]) {
    for (int i = 1; i < 4; i++) {
        for (int j = 0; j < i; j++) {
            unsigned char temp = data_matrix[i][0];
            for (int k = 0; k < 3; k++) {
                data_matrix[i][k] = data_matrix[i][k + 1];
            }
            data_matrix[i][3] = temp;
        }
    }
}

static void mix_columns(unsigned char data_matrix[4][4]) {
    unsigned char temp_matrix[4][4];
    for (int i = 0; i < 4; i++) {
        temp_matrix[0][i] = (unsigned char)(2 * data_matrix[0][i] ^ 3 * data_matrix[1][i] ^ data_matrix[2][i] ^ data_matrix[3][i]);
        temp_matrix[1][i] = (unsigned char)(data_matrix[0][i] ^ 2 * data_matrix[1][i] ^ 3 * data_matrix[2][i] ^ data_matrix[3][i]);
        temp_matrix[2][i] = (unsigned char)(data_matrix[0][i] ^ data_matrix[1][i] ^ 2 * data_matrix[2][i] ^ 3 * data_matrix[3][i]);
        temp_matrix[3][i] = (unsigned char)(3 * data_matrix[0][i] ^ data_matrix[1][i] ^ data_matrix[2][i] ^ 2 * data_matrix[3][i]);
    }
    memcpy(data_matrix, temp_matrix, 16);
}

//inverse functions

static void inv_shift_rows(unsigned char data_matrix[4][4]) {
    for (int i = 1; i < 4; i++) {
        for (int j = 0; j < i; j++) {
            unsigned char temp = data_matrix[i][3];
            for (int k = 3; k > 0; k--) {
                data_matrix[i][k] = data_matrix[i][k - 1];
            }
            data_matrix[i][0] = temp;
        }
    }
}

static void inv_mix_columns(unsigned char data_matrix[4][4]) {
    unsigned char temp_matrix[4][4];
    for (int i = 0; i < 4; i++) {
        temp_matrix[0][i] = (unsigned char)(0x0e * data_matrix[0][i] ^ 0x0b * data_matrix[1][i] ^ 0x0d * data_matrix[2][i] ^ 0x09 * data_matrix[3][i]);
        temp_matrix[1][i] = (unsigned char)(0x09 * data_matrix[0][i] ^ 0x0e * data_matrix[1][i] ^ 0x0b * data_matrix[2][i] ^ 0x0d * data_matrix[3][i]);
        temp_matrix[2][i] = (unsigned char)(0x0d * data_matrix[0][i] ^ 0x09 * data_matrix[1][i] ^ 0x0e * data_matrix[2][i] ^ 0x0b * data_matrix[3][i]);
        temp_matrix[3][i] = (unsigned char)(0x0b * data_matrix[0][i] ^ 0x0d * data_matrix[1][i] ^ 0x09 * data_matrix[2][i] ^ 0x0e * data_matrix[3][i]);
    }
    memcpy(data_matrix, temp_matrix, 16);
}

static void remove_padding(unsigned char *data, unsigned int data_len) {
    unsigned char padding = data[data_len - 1];
    for (int i = 0; i < padding; i++) {
        data[data_len - 1 - i] = 0;
    }
}

static void inv_sub_bytes(unsigned char *data, unsigned char *sbox) {
    for (int i = 0; i < 16; i++) {
        data[i] = sbox[data[i]];
    }
}


// now we will implement the encryption and decryption functions

static void AES_Encrypt(unsigned char *data, unsigned char *key) {

    unsigned char sbox[256];
    generate_sbox(sbox, key);

    unsigned char expanded_key[176];
    KeyExpansion(key, expanded_key, sbox);

    unsigned char data_matrix[4][4];
    generate_matrix(data, data_matrix);

    unsigned char key_matrix[4][4];
    generate_matrix(expanded_key, key_matrix);

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            data_matrix[i][j] ^= key_matrix[i][j];
        }
    }

    for (int i = 1; i < 10; i++) {

        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
                data_matrix[j][k] = sbox[data_matrix[j][k]];
            }
        }

        shift_rows(data_matrix);

        mix_columns(data_matrix);

        generate_matrix(expanded_key + i * 16, key_matrix);

        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
                data_matrix[j][k] ^= key_matrix[j][k];
            }
        }
    }

    for (int j = 0; j < 4; j++) {
        for (int k = 0; k < 4; k++) {
            data_matrix[j][k] = sbox[data_matrix[j][k]];
        }
    }

    shift_rows(data_matrix);

    generate_matrix(expanded_key + 160, key_matrix);

    for (int j = 0; j < 4; j++) {
        for (int k = 0; k < 4; k++) {
            data_matrix[j][k] ^= key_matrix[j][k];
        }
    }

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            data[i * 4 + j] = data_matrix[j][i];
        }
    }
}

static void AES_Decrypt(unsigned char *data, unsigned char *key) {

    unsigned char sbox[256];
    generate_sbox(sbox, key);

    unsigned char expanded_key[176];
    KeyExpansion(key, expanded_key, sbox);

    unsigned char data_matrix[4][4];
    generate_matrix(data, data_matrix);

    unsigned char key_matrix[4][4];
    generate_matrix(expanded_key + 160, key_matrix);

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            data_matrix[i][j] ^= key_matrix[i][j];
        }
    }

    inv_shift_rows(data_matrix);

    for (int i = 1; i < 10; i++) {

        generate_matrix(expanded_key + (10 - i) * 16, key_matrix);

        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
                data_matrix[j][k] ^= key_matrix[j][k];
            }
        }

        inv_mix_columns(data_matrix);

        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
                data_matrix[j][k] = sbox[data_matrix[j][k]];
            }
        }

        inv_shift_rows(data_matrix);
    }

    generate_matrix(expanded_key, key_matrix);

    for (int j = 0; j < 4; j++) {
        for (int k = 0; k < 4; k++) {
            data_matrix[j][k] ^= key_matrix[j][k];
        }
    }

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            data[i * 4 + j] = data_matrix[j][i];
        }
    }
}


// we will have a function that sets up the needed data for the encryption

void AES_PCBC_Setup(AES_PCBC *aes_pcbc, AES_PCBC_Data *key, AES_PCBC_Data *iv) {
    convert_to_16b(key->data, key->data_len, 0);
    count += 1;
    convert_to_16b(iv->data, iv->data_len, count);
    memcpy(aes_pcbc->key, key->data, 16);
    memcpy(aes_pcbc->iv, iv->data, 16);
}

void AES_PCBC_Encrypt(AES_PCBC *aes_pcbc, unsigned char *data, unsigned int data_len) {
    unsigned char *iv = aes_pcbc->iv;
    unsigned char *key = aes_pcbc->key;
    unsigned char iv_copy[16];
    memcpy(iv_copy, iv, 16);
    unsigned char key_copy[16];
    memcpy(key_copy, key, 16);
    unsigned char data_copy[data_len];
    memcpy(data_copy, data, data_len);
    pad_data(data_copy, data_len);
    data_len += data_len % 16 == 0 ? 0 : 16 - (data_len % 16);
    unsigned int blocks = data_len / 16;
    for (int i = 0; i < blocks; i++) {
        unsigned char temp[16];
        memcpy(temp, data_copy + i * 16, 16);
        xor_block(data_copy + i * 16, iv_copy);
        AES_Encrypt(data_copy + i * 16, key_copy);
        xor_block(temp, data_copy + i * 16);
        memcpy(iv_copy, temp, 16);
    }
    memcpy(data, data_copy, data_len);
}

void AES_PCBC_Decrypt(AES_PCBC *aes_pcbc, unsigned char *data, unsigned int data_len) {
    unsigned char *iv = aes_pcbc->iv;
    unsigned char *key = aes_pcbc->key;
    unsigned char iv_copy[16];
    memcpy(iv_copy, iv, 16);
    unsigned char key_copy[16];
    memcpy(key_copy, key, 16);
    unsigned char data_copy[data_len];
    memcpy(data_copy, data, data_len);
    unsigned int blocks = data_len / 16;
    for (int i = 0; i < blocks; i++) {
        unsigned char temp[16];
        memcpy(temp, data_copy + i * 16, 16);
        AES_Decrypt(data_copy + i * 16, key_copy);
        xor_block(data_copy + i * 16, iv_copy);
        xor_block(temp, data_copy + i * 16);
        memcpy(iv_copy, temp, 16);
    }
    remove_padding(data_copy, data_len);
    memcpy(data, data_copy, data_len);
}































// static void shift_rows_custom(unsigned char data_matrix[4][4], unsigned char key_matrix[4][4]) {
//     for (int k = 0; k < 4; k++) {
//         unsigned char key_values[4];
//         for (int i = 0; i < 4; i++) {
//             key_values[i] = key_matrix[i][k];
//         }
//         unsigned char key_indices[4] = {0, 1, 2, 3};
//         for (int i = 0; i < 4; i++) {
//             for (int j = i + 1; j < 4; j++) {
//                 if (key_values[i] > key_values[j]) {
//                     unsigned char temp = key_values[i];
//                     key_values[i] = key_values[j];
//                     key_values[j] = temp;
//                     temp = key_indices[i];
//                     key_indices[i] = key_indices[j];
//                     key_indices[j] = temp;
//                 }
//             }
//         }
//         for (int i = 0; i < 4; i++) {
//             data_matrix[k][key_indices[i]] = data_matrix[k][i];
//         }
//     }
// }
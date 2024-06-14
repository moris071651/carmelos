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
    data[data_len + padding_len] = '\0';
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
    word[0] ^= (unsigned char)(0x01 << (i - 1));
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
    key[16] = '\0';

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

static void geneate_inv_sbox(unsigned char *sbox, unsigned char *inv_sbox) {
    for (int i = 0; i < 256; i++) {
        inv_sbox[sbox[i]] = (unsigned char)i;
    }
}

static void generate_mix_matrix(unsigned char mix_matrix[4][4]) {
    mix_matrix[0][0] = 0x02;
    mix_matrix[0][1] = 0x03;
    mix_matrix[0][2] = 0x01;
    mix_matrix[0][3] = 0x01;
    mix_matrix[1][0] = 0x01;
    mix_matrix[1][1] = 0x02;
    mix_matrix[1][2] = 0x03;
    mix_matrix[1][3] = 0x01;
    mix_matrix[2][0] = 0x01;
    mix_matrix[2][1] = 0x01;
    mix_matrix[2][2] = 0x02;
    mix_matrix[2][3] = 0x03;
    mix_matrix[3][0] = 0x03;
    mix_matrix[3][1] = 0x01;
    mix_matrix[3][2] = 0x01;
    mix_matrix[3][3] = 0x02;
}

static void generate_inv_mix_matrix(unsigned char inv_mix_matrix[4][4]) {
    inv_mix_matrix[0][0] = 0x0e;
    inv_mix_matrix[0][1] = 0x0b;
    inv_mix_matrix[0][2] = 0x0d;
    inv_mix_matrix[0][3] = 0x09;
    inv_mix_matrix[1][0] = 0x09;
    inv_mix_matrix[1][1] = 0x0e;
    inv_mix_matrix[1][2] = 0x0b;
    inv_mix_matrix[1][3] = 0x0d;
    inv_mix_matrix[2][0] = 0x0d;
    inv_mix_matrix[2][1] = 0x09;
    inv_mix_matrix[2][2] = 0x0e;
    inv_mix_matrix[2][3] = 0x0b;
    inv_mix_matrix[3][0] = 0x0b;
    inv_mix_matrix[3][1] = 0x0d;
    inv_mix_matrix[3][2] = 0x09;
    inv_mix_matrix[3][3] = 0x0e;
}

unsigned char gal_mul(unsigned char a, unsigned char b) {
    unsigned char p = 0;
    for (int i = 0; i < 8; i++) {
        if (b & 1) {
            p ^= a;
        }
        unsigned char carry = a & 0x80;
        a <<= 1;
        if (carry) {
            a ^= 0x1b;
        }
        b >>= 1;
    }
    return p;
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
    unsigned char mix_matrix[4][4];
    generate_mix_matrix(mix_matrix);
    for (int i = 0; i < 4; i++) {
        temp_matrix[0][i] = (unsigned char)(gal_mul(mix_matrix[0][0], data_matrix[0][i]) ^ gal_mul(mix_matrix[0][1], data_matrix[1][i]) ^ gal_mul(mix_matrix[0][2], data_matrix[2][i]) ^ gal_mul(mix_matrix[0][3], data_matrix[3][i]));
        temp_matrix[1][i] = (unsigned char)(gal_mul(mix_matrix[1][0], data_matrix[0][i]) ^ gal_mul(mix_matrix[1][1], data_matrix[1][i]) ^ gal_mul(mix_matrix[1][2], data_matrix[2][i]) ^ gal_mul(mix_matrix[1][3], data_matrix[3][i]));
        temp_matrix[2][i] = (unsigned char)(gal_mul(mix_matrix[2][0], data_matrix[0][i]) ^ gal_mul(mix_matrix[2][1], data_matrix[1][i]) ^ gal_mul(mix_matrix[2][2], data_matrix[2][i]) ^ gal_mul(mix_matrix[2][3], data_matrix[3][i]));
        temp_matrix[3][i] = (unsigned char)(gal_mul(mix_matrix[3][0], data_matrix[0][i]) ^ gal_mul(mix_matrix[3][1], data_matrix[1][i]) ^ gal_mul(mix_matrix[3][2], data_matrix[2][i]) ^ gal_mul(mix_matrix[3][3], data_matrix[3][i]));
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
    unsigned char inv_mix_matrix[4][4];
    generate_inv_mix_matrix(inv_mix_matrix);
    for (int i = 0; i < 4; i++) {
        temp_matrix[0][i] = (unsigned char)(gal_mul(inv_mix_matrix[0][0], data_matrix[0][i]) ^ gal_mul(inv_mix_matrix[0][1], data_matrix[1][i]) ^ gal_mul(inv_mix_matrix[0][2], data_matrix[2][i]) ^ gal_mul(inv_mix_matrix[0][3], data_matrix[3][i]));
        temp_matrix[1][i] = (unsigned char)(gal_mul(inv_mix_matrix[1][0], data_matrix[0][i]) ^ gal_mul(inv_mix_matrix[1][1], data_matrix[1][i]) ^ gal_mul(inv_mix_matrix[1][2], data_matrix[2][i]) ^ gal_mul(inv_mix_matrix[1][3], data_matrix[3][i]));
        temp_matrix[2][i] = (unsigned char)(gal_mul(inv_mix_matrix[2][0], data_matrix[0][i]) ^ gal_mul(inv_mix_matrix[2][1], data_matrix[1][i]) ^ gal_mul(inv_mix_matrix[2][2], data_matrix[2][i]) ^ gal_mul(inv_mix_matrix[2][3], data_matrix[3][i]));
        temp_matrix[3][i] = (unsigned char)(gal_mul(inv_mix_matrix[3][0], data_matrix[0][i]) ^ gal_mul(inv_mix_matrix[3][1], data_matrix[1][i]) ^ gal_mul(inv_mix_matrix[3][2], data_matrix[2][i]) ^ gal_mul(inv_mix_matrix[3][3], data_matrix[3][i]));
    }
    memcpy(data_matrix, temp_matrix, 16);
}

static void remove_padding(unsigned char *data, unsigned int data_len) {
    unsigned char padding = data[data_len - 1];
    for (int i = 0; i < padding; i++) {
        data[data_len - 1 - i] = 0;
    }
}

static void inv_sub_bytes(unsigned char data_matrix[4][4], unsigned char *sbox) {
    unsigned char inv_sbox[256];
    geneate_inv_sbox(sbox, inv_sbox);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            data_matrix[i][j] = inv_sbox[data_matrix[i][j]];
        }
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

    inv_sub_bytes(data_matrix, sbox);

    for (int i = 1; i < 10; i++) {

        generate_matrix(expanded_key + (10 - i) * 16, key_matrix);

        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
                data_matrix[j][k] ^= key_matrix[j][k];
            }
        }

        inv_mix_columns(data_matrix);

        inv_shift_rows(data_matrix);

        inv_sub_bytes(data_matrix, sbox);
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
    unsigned char key_copy[key->data_len + 1 < 17 ? 17 : key->data_len + 1];
    strcpy(key_copy, key->data);
    unsigned char iv_copy[iv->data_len + 1 < 17 ? 17 : iv->data_len + 1];
    strcpy(iv_copy, iv->data);

    convert_to_16b(key_copy, key->data_len, 0);
    convert_to_16b(iv_copy, iv->data_len, count);
    count += 1;
    memcpy(aes_pcbc->key, key_copy, 17);
    memcpy(aes_pcbc->iv, iv_copy, 17);
}

void AES_PCBC_Encrypt(AES_PCBC *aes_pcbc, unsigned char *data, unsigned int data_len) {
    unsigned char *iv = aes_pcbc->iv;
    unsigned char *key = aes_pcbc->key;
    unsigned char iv_copy[17];
    memcpy(iv_copy, iv, 16);
    iv_copy[16] = '\0';
    unsigned char key_copy[17];
    memcpy(key_copy, key, 16);
    key_copy[16] = '\0';
    unsigned char data_copy[data_len + 1];
    memcpy(data_copy, data, data_len);
    data_copy[data_len] = '\0';
    pad_data(data_copy, data_len);
    data_len += data_len % 16 == 0 ? 0 : 16 - (data_len % 16);
    unsigned int blocks = data_len / 16;
    for (int i = 0; i < blocks; i++) {
        unsigned char temp[17];
        memcpy(temp, data_copy + i * 16, 16);
        temp[16] = '\0';
        xor_block(data_copy + i * 16, iv_copy);
        //printf("Data: %s\n", data_copy);
        AES_Encrypt(data_copy + i * 16, key_copy);
        xor_block(temp, data_copy + i * 16);
        memcpy(iv_copy, temp, 16);
        iv_copy[16] = '\0';
    }
    memcpy(data, data_copy, data_len);
}

void AES_PCBC_Decrypt(AES_PCBC *aes_pcbc, unsigned char *data, unsigned int data_len) {
    unsigned char *iv = aes_pcbc->iv;
    unsigned char *key = aes_pcbc->key;
    unsigned char iv_copy[17];
    memcpy(iv_copy, iv, 16);
    iv_copy[16] = '\0';
    unsigned char key_copy[17];
    memcpy(key_copy, key, 16);
    key_copy[16] = '\0';
    unsigned char data_copy[data_len + 1];
    memcpy(data_copy, data, data_len);
    data_copy[data_len] = '\0';
    unsigned int blocks = data_len / 16;
    for (int i = 0; i < blocks; i++) {
        unsigned char temp[17];
        memcpy(temp, data_copy + i * 16, 16);
        temp[16] = '\0';
        AES_Decrypt(data_copy + i * 16, key_copy);
        xor_block(data_copy + i * 16, iv_copy);
        xor_block(temp, data_copy + i * 16);
        memcpy(iv_copy, temp, 16);
        iv_copy[16] = '\0';
    }
    remove_padding(data_copy, data_len);
    memcpy(data, data_copy, data_len);
}




void Test_parts() {
    //just for tests - UPDATE: If the encryption is broken it is not my fault, tested and fixed everything today
    unsigned char key[17] = "1234567890123456";
    unsigned char iv[17] = "1234567890123456";
    unsigned char data[16] = "Hello, World!!!";
    key[16] = '\0';
    iv[16] = '\0';
    data[15] = '\0';
    unsigned int data_len = 15;
    pad_data(data, data_len);
    printf("Data: %s\n", data);
    AES_Encrypt(data, key);
    printf("Encrypted: %s\n", data);
    AES_Decrypt(data, key);
    printf("Decrypted: %s\n", data);

    //test sbox
    // unsigned char sbox[256];
    // generate_sbox(sbox, key);
    // unsigned char test[17] = "Hello, World!!!";
    // test[16] = '\0';
    // for(int i = 0; i < 15; i++) {
    //     test[i] = sbox[test[i]];
    // }
    // printf("Sbox: %s\n", test);
    // inv_sub_bytes(test, sbox);
    // printf("Inv Sbox: %s\n", test);
    //working

    //test mix columns
    // unsigned char data_matrix[4][4] = {
    //     {0x02, 0x03, 0x01, 0x01},
    //     {0x01, 0x02, 0x03, 0x01},
    //     {0x01, 0x01, 0x02, 0x03},
    //     {0x03, 0x01, 0x01, 0x02}
    // };
    // printf("Data Matrix: \n");
    // for(int i = 0; i < 4; i++) {
    //     for(int j = 0; j < 4; j++) {
    //         printf("%02x ", data_matrix[i][j]);
    //     }
    //     printf("\n");
    // }
    // mix_columns(data_matrix);
    // printf("Mix Columns: \n");
    // for(int i = 0; i < 4; i++) {
    //     for(int j = 0; j < 4; j++) {
    //         printf("%02x ", data_matrix[i][j]);
    //     }
    //     printf("\n");
    // }
    // inv_mix_columns(data_matrix);
    // printf("Inv Mix Columns: \n");
    // for(int i = 0; i < 4; i++) {
    //     for(int j = 0; j < 4; j++) {
    //         printf("%02x ", data_matrix[i][j]);
    //     }
    //     printf("\n");
    // }
    //working

    //test shift rows
    // unsigned char data_matrix[4][4] = {
    //     {0x02, 0x03, 0x01, 0x01},
    //     {0x01, 0x02, 0x03, 0x01},
    //     {0x01, 0x01, 0x02, 0x03},
    //     {0x03, 0x01, 0x01, 0x02}
    // };
    // printf("Data Matrix: \n");
    // for(int i = 0; i < 4; i++) {
    //     for(int j = 0; j < 4; j++) {
    //         printf("%02x ", data_matrix[i][j]);
    //     }
    //     printf("\n");
    // }
    // shift_rows(data_matrix);
    // printf("Shift Rows: \n");
    // for(int i = 0; i < 4; i++) {
    //     for(int j = 0; j < 4; j++) {
    //         printf("%02x ", data_matrix[i][j]);
    //     }
    //     printf("\n");
    // }
    // inv_shift_rows(data_matrix);
    // printf("Inv Shift Rows: \n");
    // for(int i = 0; i < 4; i++) {
    //     for(int j = 0; j < 4; j++) {
    //         printf("%02x ", data_matrix[i][j]);
    //     }
    //     printf("\n");
    // }
    //working
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
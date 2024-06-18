#include "CRC.h"

void CRC_Init(CRC *crc) {
    crc->crc = 0;
}

void CRC_Update(CRC *crc, unsigned char *data, unsigned int size) {
    for (unsigned int i = 0; i < size; i++) {
        crc->crc ^= data[i];
        for (unsigned int j = 0; j < 8; j++) {
            if (crc->crc & 1) {
                crc->crc = (crc->crc >> 1) ^ 0xEDB88320;
            } else {
                crc->crc >>= 1;
            }
        }
    }
}

unsigned int CRC_Final(CRC *crc) {
    return crc->crc;
}
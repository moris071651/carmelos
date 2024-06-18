#ifndef CRC_H
#define CRC_H

typedef struct {
    unsigned int crc;
} CRC;

void CRC_Init(CRC *crc);

void CRC_Update(CRC *crc, unsigned char *data, unsigned int size);

unsigned int CRC_Final(CRC *crc);

#endif
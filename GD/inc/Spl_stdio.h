#ifndef __SPL_STDIO_H
#define __SPL_STDIO_H

#include "gd32e23x.h"
#include "gd32e235c_eval.h"

void hexdump(unsigned char *data, int len);
void *memset( void *pvDest, uint8_t iValue, int ulBytes);
unsigned int ENDIAN_CONVERT_32(unsigned int x);
unsigned int crc_16_l_calc(unsigned char *buf_ptr,unsigned int len);
unsigned short ENDIAN_CONVERT_16(unsigned short x);
void *memcpy( void *pvDest, const void *pvSource, uint32_t ulBytes);
int memcmp( const void *pvMem1, const void *pvMem2, uint32_t ulBytes);

#endif

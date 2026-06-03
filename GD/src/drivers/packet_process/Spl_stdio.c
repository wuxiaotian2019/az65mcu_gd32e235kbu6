#include <stdio.h>
#include "Spl_stdio.h"

#define CRC_16_POLYNOMIAL       0x1021
#define CRC_16_L_POLYNOMIAL     0x8000
#define CRC_16_L_SEED           0x80
#define CRC_16_L_OK             0x00

void hexdump(unsigned char *data, int len)
{
  int i, j;
  int num = len/16 + 1;

  printf("dump 0x%x bytes\n", len);
  for (i = 0; i < num; i++) {
    for (j = 0; j < 16; j++) {
      if (i * 16 + j >= len)
        goto end;
      printf("%02x ", data[i*16 + j]);
    }

  }
end:
  printf("\n");
}

unsigned int crc_16_l_calc(unsigned char *buf_ptr,unsigned int len)
{
    unsigned int i=0;
    unsigned short crc = 0;
    
    while ((len--)!=0)
    {
        for(i = CRC_16_L_SEED; i !=0 ; i = i>>1)
        {
           if((crc & CRC_16_L_POLYNOMIAL) !=0 )
           {
               crc = crc << 1 ;
               crc = crc ^ CRC_16_POLYNOMIAL;
           }
           else
           {
               crc = crc << 1 ;
           } 
        
           if((((unsigned char)(*buf_ptr)) & i) != 0 )
           {
              crc = crc ^ CRC_16_POLYNOMIAL;
           }
        }
        buf_ptr++;
    }
    
    return (crc);
}


unsigned short ENDIAN_CONVERT_16(unsigned short x)
{
	unsigned short i;
	i= (((x & 0xFF) << 8) | ((x & 0xFF00 ) >> 8));
	return i;
}

 unsigned int ENDIAN_CONVERT_32(unsigned int x)
{
	unsigned int i,j;
	i=x^((x<<16)|(x>>16));
	i=i&0xff00ffff;
	j=(x<<24)|(x>>8);
	return j^(i>>8);
}

void *memset( void *pvDest, uint8_t iValue, int ulBytes )
{
	unsigned char *pcDest = ( unsigned char * ) pvDest;
	int x;

	for( x = 0; x < ulBytes; x++ )
	{
		*pcDest = ( unsigned char ) iValue;
		pcDest++;
	}

	return pvDest;
}

void *memcpy( void *pvDest, const void *pvSource, uint32_t ulBytes )
{
	unsigned char *pcDest = ( unsigned char * ) pvDest, *pcSource = ( unsigned char * ) pvSource;
	uint32_t x;

	for( x = 0; x < ulBytes; x++ )
	{
		*pcDest = *pcSource;
		pcDest++;
		pcSource++;
	}

	return pvDest;
}
        
int memcmp( const void *pvMem1, const void *pvMem2, uint32_t ulBytes )
{
	const unsigned char *pucMem1 = pvMem1, *pucMem2 = pvMem2;
	uint32_t x;

    for( x = 0; x < ulBytes; x++ )
    {
        if( pucMem1[ x ] != pucMem2[ x ] )
        {
            break;
        }
    }

    return ulBytes - x;
}
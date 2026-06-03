/*
 * SPDX-FileCopyrightText: 2026 Unisoc (Shanghai) Technologies Co., Ltd
 * SPDX-License-Identifier: LicenseRef-Unisoc-General-1.0
 *
 * Copyright 2026 Unisoc (Shanghai) Technologies Co., Ltd
 * Licensed under the Unisoc General Software License, version 1.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * https://www.unisoc.com/en_us/license/UNISOC_GENERAL_LICENSE_V1.0-EN_US
 * Software distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 * See the Unisoc General Software License, version 1.0 for more details.
 */

#ifndef _SPI_NAND_DRIVER_H
#define _SPI_NAND_DRIVER_H

#include "gd32e23x.h"
//#include "gd32e235c_eval.h"
#include <stdint.h>

/* NAND memory command */
#define NAND_WRITEENABLE				0x06
#define NAND_WEITEDISABLE				0x04
#define NAND_GETREGS					0x0F
#define NAND_SETREGS					0x1F
#define NAND_PAGEREADTOCATCH				0x13
#define NAND_NEXTPAGECACHEREAD				0x31
#define NAND_LASTPAGECACHEREAD				0x3F
#define NAND_READFROMCATCH				0x03
#define NAND_READID					0x9F
#define NAND_PROGRAMLOAD				0x02
#define NAND_PROGRAMEXECUTE				0x10
#define NAND_PROGRAMLOADRANDOMDATA			0x84
#define NAND_BLOCKERASE					0xD8
#define NAND_RESET					0xFF
#define DUMMY_BYTE					0xA5

#define NAND_OK	0
#define NAND_FAIL 1

#ifndef READ_NANDFLASH_ECC_ERROR
#define READ_NANDFLASH_ECC_ERROR			0x9A
#endif
/* NAND regs add */
#define NAND_REGS_PROTECTION				0xA0
#define NAND_REGS_FEATURE				0xB0
#define NAND_REGS_STATUS				0xC0

#define SPI_NANDFLASH_CS_LOW()		gpio_bit_reset(GPIOA,GPIO_PIN_4)
#define SPI_NANDFLASH_CS_HIGH()		while(RESET != spi_i2s_flag_get(SPI0,SPI_FLAG_TRANS));\
				gpio_bit_set(GPIOA,GPIO_PIN_4) //

extern int NAND_PAGE_SIZE;
extern int NAND_PAGE_PER_BLK;
/* NAND Status */
/* ECC results: */
/* 00 No errors	*/
/* 01 Bit error detected and corrected, error bit number = 1~7 */
/* 10 Bit errors greater than 8 bits detected and not corrected	*/
/* 11 Bit error detected and corrected, error bit number = 8 */
#define NAND_STATUS_ECCS2				0x40
#define NAND_STATUS_ECCS1				0x20
#define NAND_STATUS_ECCS0				0x10
#define NAND_STATUS_PROGRAMFAIL				0x08	/* program fail */
#define NAND_STATUS_ERASEFAIL				0x04	/* erase fail */
#define NAND_STATUS_WEL					0x02	/* write enable */
/* Busy:PROGRAM EXECUTE, PAGE READ, BLOCK ERASE, RESET */
#define NAND_STATUS_QIP					0x01

typedef struct __SPARE_AREA {
 uint16_t BlockStatus;
 uint16_t LogicalIndex;
}SPARE_AREA;

typedef struct
{
 uint16_t Block;
 uint16_t Page;
 uint16_t Area;
} NAND_ADDRESS;

typedef struct
{
 uint8_t Maker_ID;
 uint8_t Device_ID;
}NAND_IDTypeDef;

typedef struct{
	void (*spinand_setregs)(uint8_t regs, uint8_t data);
	uint8_t (*spinand_getregs)(uint8_t regs);
	void (*spinand_readfromcatch)(uint8_t *pBuffer, NAND_ADDRESS Address, uint32_t length);
	void (*spinand_readtocatch)(NAND_ADDRESS Address);
	void (*spinand_writeenable)(void);
	uint8_t (*spinand_blockerase)(uint32_t BlockID);
	void (*spinand_eccenable)(void);
	void (*spinand_eccdisable)(void);
	uint8_t (*spinand_checkecc)(void);
	void (*spinand_config)(void);
	void (*spinand_recovery)(void);
	void (*spinand_check_blockstatus)(NAND_ADDRESS address,SPARE_AREA *block_statue);
}SPINAND_OPERATIONS_T;

typedef struct{
	uint8_t dev_id[5];	// ID of spinand driver product ic
	SPINAND_OPERATIONS_T *spinnad_operation;	//spi nand operation
	uint32_t pagesize;
	uint32_t page_per_blk;
}SPINAND_CFG_INFO_T;

int spi_nandflash_init(uint32_t polarity_phase_clock);
uint8_t spi_nand_rw_byte(uint8_t byte);
#define true 1
#define false 0
static inline void spi_nand_cs_sel(uint8_t sel)
{
	if (sel) {
		SPI_NANDFLASH_CS_LOW();
		return;
	}
	SPI_NANDFLASH_CS_HIGH();
}
SPINAND_CFG_INFO_T * spinand_get_specinfo(void);
void spi_nand_reset(void);
uint16_t NANDFLASH_Read(NAND_ADDRESS add, uint8_t *Readbuff, uint16_t Transfer_Length);

#endif

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

#include "GD5F1GQ4xExxG.h"
#include "gd32e23x.h"
#include <stdio.h>
#include "systick.h"

/**
 * @brief Set NAND flash feature
 * @param regs feature address
 * @param data Data to write
 */
void spi_nand_set_feature(uint8_t regs, uint8_t data)
{
	spi_nand_cs_sel(true);

	spi_nand_rw_byte(NAND_SETREGS);
	spi_nand_rw_byte(regs);
	spi_nand_rw_byte(data);

	spi_nand_cs_sel(false);
}

/**
 * @brief Get NAND flash feature value
 * @param regs feature address
 * @return feature value
 */
uint8_t spi_nand_get_feature(uint8_t regs)
{
	uint8_t temp = 0;

	spi_nand_cs_sel(true);

	spi_nand_rw_byte(NAND_GETREGS);
	spi_nand_rw_byte(regs);
	temp = spi_nand_rw_byte(DUMMY_BYTE);

	spi_nand_cs_sel(false);

	return temp;
}

/**
 * @brief Read data from NAND flash cache
 * @param pBuffer Buffer to store data
 * @param Address Address to read from
 * @param length Number of bytes to read
 */
void spi_nand_read_from_cache(uint8_t *pBuffer, NAND_ADDRESS Address, uint32_t length)
{
	uint32_t i = 0;

	spi_nand_cs_sel(true);
	spi_nand_rw_byte(NAND_READFROMCATCH);
	spi_nand_rw_byte((Address.Area >> 8) & 0xFF);
	spi_nand_rw_byte((Address.Area) & 0xFF);
	spi_nand_rw_byte(DUMMY_BYTE);

	for(i = 0; i < length; i++)
		*pBuffer++ = spi_nand_rw_byte(DUMMY_BYTE);

	spi_nand_cs_sel(false);
}

/**
 * @brief Read data to NAND flash cache
 * @param Address Address to read from
 */
void spi_nand_read2cache(NAND_ADDRESS Address)
{
	uint32_t block_add = ((Address.Block & 0x3ff) << 6) | (Address.Page & 0x3f);

	spi_nand_cs_sel(true);

	spi_nand_rw_byte(NAND_PAGEREADTOCATCH);
	spi_nand_rw_byte((uint8_t)((block_add) >> 16));
	spi_nand_rw_byte((uint8_t)((block_add) >> 8));
	spi_nand_rw_byte((uint8_t)block_add);

	spi_nand_cs_sel(false);

	while((spi_nand_get_feature(NAND_REGS_STATUS) & NAND_STATUS_QIP) != 0);
}

/**
 * @brief Enable NAND flash write
 */
void spi_nand_write_enable(void)
{
	spi_nand_cs_sel(true);

	spi_nand_rw_byte(NAND_WRITEENABLE);

	spi_nand_cs_sel(false);
}

/**
 * @brief Execute NAND flash program
 * @param WriteAddr Write address
 * @return Status register value
 */
static uint8_t spi_nand_program_excute(uint32_t WriteAddr)
{
	spi_nand_write_enable();

	spi_nand_cs_sel(true);

	spi_nand_rw_byte(NAND_PROGRAMEXECUTE);
	spi_nand_rw_byte((WriteAddr & 0xFF0000) >> 16);
	spi_nand_rw_byte((WriteAddr & 0xFF00) >> 8);
	spi_nand_rw_byte(WriteAddr & 0xFF);

	spi_nand_cs_sel(false);

	while((spi_nand_get_feature(NAND_REGS_STATUS) & NAND_STATUS_QIP) != 0);

	return spi_nand_get_feature(NAND_REGS_STATUS);
}

/**
 * @brief Load data to NAND flash program buffer
 * @param pBuffer Data buffer
 * @param WriteAddr Write address
 * @param NumByteToWrite Number of bytes to write
 */
static void spi_nand_page_program(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
	spi_nand_cs_sel(true);

	spi_nand_rw_byte(NAND_PROGRAMLOAD);
	spi_nand_rw_byte((WriteAddr & 0xFF00) >> 8);
	spi_nand_rw_byte(WriteAddr & 0xFF);

	while (NumByteToWrite--) {
		spi_nand_rw_byte(*pBuffer);
		pBuffer++;
	}

	spi_nand_cs_sel(false);
}

/**
 * @brief Write data to NAND flash
 * @param Address Write address
 * @param pBuffer Data buffer
 * @param DataLen Number of bytes to write
 * @return Write status
 */
static uint8_t spi_nand_write_page(NAND_ADDRESS Address, uint8_t* pBuffer, uint16_t DataLen)
{
	uint32_t block_add = ((Address.Block & 0x3ff) << 6) | (Address.Page & 0x3f);

	spi_nand_page_program(pBuffer, Address.Area, DataLen);

	return spi_nand_program_excute(block_add);
}

/**
 * @brief Enable NAND flash ECC
 */
void spi_nand_enable_ecc(void)
{
	uint8_t getStatus = 0;

	getStatus = spi_nand_get_feature(NAND_REGS_FEATURE);
	getStatus |= 1 << 4;
	spi_nand_set_feature(NAND_REGS_FEATURE, getStatus);
}

/**
 * @brief Disable NAND flash ECC
 */
void spi_nand_disable_ecc(void)
{
	uint8_t getStatus = 0;

	getStatus = spi_nand_get_feature(NAND_REGS_FEATURE);
	getStatus &= ~(1 << 4);
	spi_nand_set_feature(NAND_REGS_FEATURE, getStatus);
}

/**
 * @brief Check NAND flash ECC status
 * @return NAND_OK if no error, NAND_FAIL if error
 */
uint8_t spi_nand_check_ecc(void)
{
	uint8_t getStatus = 0;

	getStatus = spi_nand_get_feature(NAND_REGS_FEATURE);
	if(((getStatus & NAND_STATUS_ECCS1) == NAND_STATUS_ECCS1) &&
		((getStatus & NAND_STATUS_ECCS0) == 0))
		return NAND_FAIL;

	return NAND_OK;
}

/**
 * @brief Erase NAND flash block
 * @param BlockID Block number to erase
 * @return Status register value
 */
uint8_t spi_nand_erase_blk(uint32_t BlockID)
{
	uint32_t Blockaddr = ((BlockID & 0x3ff) << 6);

	spi_nand_write_enable();

	spi_nand_cs_sel(true);

	spi_nand_rw_byte(NAND_BLOCKERASE);
	spi_nand_rw_byte((Blockaddr & 0xFF0000) >> 16);
	spi_nand_rw_byte((Blockaddr & 0xFF00) >> 8);
	spi_nand_rw_byte(Blockaddr & 0xFF);

	spi_nand_cs_sel(false);

	while((spi_nand_get_feature(NAND_REGS_STATUS) & NAND_STATUS_QIP) != 0);

	return spi_nand_get_feature(NAND_REGS_STATUS);
}

void spi_nand_config(void)
{
	spi_nand_reset();
	delay_1ms(2);
}

void spi_nand_check_blk_sts(NAND_ADDRESS address, SPARE_AREA *block_statue)
{
	uint8_t Buffer[4];

	address.Page = 0;
	spi_nand_read2cache(address);
	spi_nand_read_from_cache(Buffer, address, 4);
	*block_statue = *(SPARE_AREA *)Buffer;
}

SPINAND_OPERATIONS_T g_spinand_gd = {
	spi_nand_set_feature,
	spi_nand_get_feature,
	spi_nand_read_from_cache,
	spi_nand_read2cache,
	spi_nand_write_enable,
	spi_nand_erase_blk,
	spi_nand_enable_ecc,
	spi_nand_disable_ecc,
	spi_nand_check_ecc,
	spi_nand_config,
	NULL,
	spi_nand_check_blk_sts,
};

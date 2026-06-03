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

#ifndef _GD5F1GQ4XEXXG_H
#define _GD5F1GQ4XEXXG_H

#include "gd32e23x.h"
#include "Spi_nand_driver.h"

typedef enum
{
	First_Page,
	Continue_Page,
	Last_Page,
} page_type_t;

void spi_nand_check_blk_sts(NAND_ADDRESS address,SPARE_AREA *block_statue);
uint8_t spi_nand_get_feature(uint8_t regs);
void spi_nand_set_feature(uint8_t regs, uint8_t data);
void spi_nand_read_from_cache (uint8_t *pBuffer, NAND_ADDRESS Address, uint32_t length);
void spi_nand_read2cache(NAND_ADDRESS Address);
void spi_nand_write_enable(void);
uint8_t spi_nand_erase_blk(uint32_t BlockID);
void spi_nand_enable_ecc(void);
void spi_nand_disable_ecc(void);
uint8_t spi_nand_check_ecc(void);
uint8_t spi_nand_erase_blk(uint32_t BlockID);
void spi_nand_config(void);
#endif

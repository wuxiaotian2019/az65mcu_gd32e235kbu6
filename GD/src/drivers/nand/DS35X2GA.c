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

#include <stdio.h>
#include "DS35X2GA.h"
#include "GD5F1GQ4xExxG.h"

void ds_spi_nand_read_from_cache (uint8_t *pBuffer, NAND_ADDRESS Address, uint32_t length)
{
	uint32_t i = 0;

	spi_nand_cs_sel(true);

	if (Address.Block == 1)
		Address.Area |= (1 << 12);

	spi_nand_rw_byte(NAND_READFROMCATCH);
	//spi_nand_rw_byte(DUMMY_BYTE);
	spi_nand_rw_byte((Address.Area >>8) & 0xFF);
	spi_nand_rw_byte((Address.Area) & 0xFF);
	spi_nand_rw_byte(DUMMY_BYTE);

	for(i = 0; i < length; i++)
		*pBuffer++ = spi_nand_rw_byte(DUMMY_BYTE);

	spi_nand_cs_sel(false);
}

SPINAND_OPERATIONS_T g_spinand_ds = {
	spi_nand_set_feature,
	spi_nand_get_feature,
	ds_spi_nand_read_from_cache,
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

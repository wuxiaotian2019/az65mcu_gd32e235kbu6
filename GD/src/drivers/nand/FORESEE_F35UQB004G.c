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
#include "GD5F1GQ4xExxG.h"

uint8_t foresee_spi_nand_check_ecc(void)
{
	uint8_t GetStatus = 0;

	GetStatus = spi_nand_get_feature(NAND_REGS_FEATURE);
	if((GetStatus & (NAND_STATUS_ECCS1 | NAND_STATUS_ECCS1 | NAND_STATUS_ECCS0))
		== (NAND_STATUS_ECCS1 | NAND_STATUS_ECCS1 | NAND_STATUS_ECCS0))
		return NAND_FAIL;

	return NAND_OK;

}

SPINAND_OPERATIONS_T g_spinand_foresee = {
	spi_nand_set_feature,
	spi_nand_get_feature,
	spi_nand_read_from_cache,
	spi_nand_read2cache,
	spi_nand_write_enable,
	spi_nand_erase_blk,
	spi_nand_enable_ecc,
	spi_nand_disable_ecc,
	foresee_spi_nand_check_ecc,
	spi_nand_config,
	NULL,
	spi_nand_check_blk_sts,
};

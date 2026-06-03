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

#include "ESMTF50D1G41MYB.h"
#include "GD5F1GQ4xExxG.h"
#include <stdio.h>
#include "systick.h"

/**
 * @brief Disable continuous read mode
 */
static void spi_nand_continue_read_disable(void)
{
	uint8_t getStatus;

	getStatus = spi_nand_get_feature(NAND_REGS_FEATURE);
	getStatus |= (1 << 3);
	spi_nand_set_feature(NAND_REGS_FEATURE, getStatus);
}

/**
 * @brief Enable continuous read mode
 */
static void spi_nand_continue_read_enable(void)
{
	uint8_t getStatus;

	getStatus = spi_nand_get_feature(NAND_REGS_FEATURE);
	getStatus &= ~(1 << 3);
	spi_nand_set_feature(NAND_REGS_FEATURE, getStatus);
}

/**
 * @brief Configure ESMT NAND flash
 */
static void esmt_spi_nand_config(void)
{
	spi_nand_reset();
	delay_1ms(2);
	spi_nand_continue_read_disable();
}

/**
 * @brief Recovery ESMT NAND flash
 */
static void esmt_spi_nand_recovery(void)
{
	spi_nand_continue_read_enable();
}

/**
 * @brief Check ECC status for ESMT NAND
 * @return NAND_OK if no error, NAND_FAIL if error
 */
static uint8_t esmt_spi_nand_check_ecc(void)
{
	uint8_t getStatus = 0;

	getStatus = spi_nand_get_feature(NAND_REGS_FEATURE);
	if ((((getStatus & NAND_STATUS_ECCS1) == NAND_STATUS_ECCS1) &&
		((getStatus & NAND_STATUS_ECCS0) == 0)) ||
		(((getStatus & NAND_STATUS_ECCS1) == NAND_STATUS_ECCS1) &&
		((getStatus & NAND_STATUS_ECCS0) == NAND_STATUS_ECCS0)))
		return NAND_FAIL;

	return NAND_OK;
}

/**
 * @brief ESMT NAND flash operations structure
 */
SPINAND_OPERATIONS_T g_spinand_esmt = {
	spi_nand_set_feature,
	spi_nand_get_feature,
	spi_nand_read_from_cache,
	spi_nand_read2cache,
	spi_nand_write_enable,
	spi_nand_erase_blk,
	spi_nand_enable_ecc,
	spi_nand_disable_ecc,
	esmt_spi_nand_check_ecc,
	esmt_spi_nand_config,
	esmt_spi_nand_recovery,
	spi_nand_check_blk_sts,
};

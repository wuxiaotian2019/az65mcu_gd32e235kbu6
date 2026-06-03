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

#include "Spi_nand_driver.h"
//#include "Process_Packet.h"
//#include "Uart_init.h"
#include <stdio.h>
//#include "Spl_stdio.h"

// Device ID definitions
#define SPINAND_ID_GD5F1GQ4R_1G		{0xc1, 0xc8, 0xc1, 0xc8, 0xc1}	// GD spi nand 1Gbit 1.8v
#define SPINAND_ID_GD5F1GQ4R_2G		{0xc2, 0xc8, 0x00, 0x00, 0x00}	// GD spi nand 2Gbit 1.8v
#define SPINAND_ID_ESMTF50D1G41MYB	{0x8c, 0x61, 0x00, 0x00, 0x00}	// ESMT spi nand 1Gbit 1.8v
#define SPINAND_ID_ESMTF50D1G41LB	{0xc8, 0x11, 0x00, 0x00, 0x00}	// ESMT spi nand 1Gbit 1.8v
#define SPINAND_ID_GD5F2GQ5		{0xc8, 0x42, 0x00, 0x00, 0x00}	// GD spi nand 1Gbit 1.8v
#define SPINAND_ID_GD5F1GQ5UE		{0xc8, 0x51, 0x00, 0x00, 0x00}	// GD spi nand 1Gbit 1.8v
#define SPINAND_ID_GD5F1GQ5RE		{0xc8, 0x41, 0x00, 0x00, 0x00}	// GD spi nand 1Gbit 1.8v
#define SPINAND_DS35X1GA		{0xE5, 0x21, 0x00, 0x00, 0x00}
#define SPINAND_DS35X2GA		{0xE5, 0x22, 0x00, 0x00, 0x00}
#define SPINAND_DS35X4GBXXX		{0xE5, 0x64, 0x00, 0x00, 0x00}
#define SPINAND_FORESEE_F35UQB004G	{0xCD, 0x43, 0x43, 0x00, 0x00}
#define SPINAND_XTX_XT26Q04D		{0x0B, 0x53, 0x00, 0x00, 0x00}
#define SPINAND_UNIM_UM19B2LISH		{0xB0,0x35,0x00,0x00,0x00}

// External operation structures
extern SPINAND_OPERATIONS_T g_spinand_gd;		// Support continue read
extern SPINAND_OPERATIONS_T g_spinand_esmt;
extern SPINAND_OPERATIONS_T g_spinand_ds;
extern SPINAND_OPERATIONS_T g_spinand_foresee;

// Global variables
SPINAND_CFG_INFO_T *s_spinand_spec_info_ptr = NULL;
uint8_t spi_nand_id[5] = {0};
int NAND_PAGE_SIZE = 2048;
int NAND_PAGE_PER_BLK = 64;

// Device configuration table
static SPINAND_CFG_INFO_T s_apinand_cfg_tab[] = {
	{SPINAND_ID_GD5F1GQ4R_1G, (SPINAND_OPERATIONS_T *)&g_spinand_gd, 2048, 64},
	{SPINAND_ID_GD5F1GQ4R_2G, (SPINAND_OPERATIONS_T *)&g_spinand_gd, 2048, 64},
	{SPINAND_ID_ESMTF50D1G41MYB, (SPINAND_OPERATIONS_T *)&g_spinand_esmt, 2048, 64},
	{SPINAND_ID_ESMTF50D1G41LB, (SPINAND_OPERATIONS_T *)&g_spinand_gd, 2048, 64},
	{SPINAND_ID_GD5F2GQ5, (SPINAND_OPERATIONS_T *)&g_spinand_gd, 2048, 64},
	{SPINAND_ID_GD5F1GQ5UE, (SPINAND_OPERATIONS_T *)&g_spinand_gd, 2048, 64},
	{SPINAND_ID_GD5F1GQ5RE, (SPINAND_OPERATIONS_T *)&g_spinand_gd, 2048, 64},
	{SPINAND_DS35X1GA, (SPINAND_OPERATIONS_T *)&g_spinand_gd, 2048, 64},
	{SPINAND_DS35X2GA, (SPINAND_OPERATIONS_T *)&g_spinand_ds, 2048, 64},
	{SPINAND_DS35X4GBXXX, (SPINAND_OPERATIONS_T *)&g_spinand_ds, 2048, 64},
	{SPINAND_FORESEE_F35UQB004G, (SPINAND_OPERATIONS_T *)&g_spinand_foresee, 4096, 64},
	{SPINAND_XTX_XT26Q04D, (SPINAND_OPERATIONS_T *)&g_spinand_ds, 4096, 64},
	{SPINAND_UNIM_UM19B2LISH,(SPINAND_OPERATIONS_T *)&g_spinand_ds, 4096, 128},
};

/**
 * @brief Initialize SPI NAND flash
 * @param polarity_phase_clock SPI clock polarity and phase settings
 * @return 0 on success, -1 on error
 */
int spi_nandflash_init(uint32_t polarity_phase_clock)
{
	spi_parameter_struct spi_init_struct;

	// Disable SPI first
	spi_disable(SPI0);

	// Enable clocks
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_SPI0);

	// Configure GPIO pins for SPI
	gpio_af_set(GPIOA, GPIO_AF_0, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
	gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE,
		      GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP,
				GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

	// Configure chip select pin
	gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_4);
	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);

	// Configure SPI parameters
	spi_init_struct.trans_mode				= SPI_TRANSMODE_FULLDUPLEX;
	spi_init_struct.device_mode				= SPI_MASTER;
	spi_init_struct.frame_size				= SPI_FRAMESIZE_8BIT;
	spi_init_struct.clock_polarity_phase			= polarity_phase_clock;
	spi_init_struct.nss					= SPI_NSS_SOFT;
	spi_init_struct.prescale				= SPI_PSC_4;
	spi_init_struct.endian					= SPI_ENDIAN_MSB;
	spi_init(SPI0, &spi_init_struct);

	// Enable SPI
	spi_enable(SPI0);

	return 0;
}

/**
 * @brief Read/write a byte via SPI
 * @param byte Byte to transmit
 * @return Received byte
 */
uint8_t spi_nand_rw_byte(uint8_t byte)
{
	// Wait for transmit buffer empty
	while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE));

	// Transmit byte
	spi_i2s_data_transmit(SPI0, byte);

	// Wait for receive buffer not empty
	while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE));

	// Return received byte
	return spi_i2s_data_receive(SPI0);
}

/**
 * @brief Reset NAND flash device
 */
void spi_nand_reset(void)
{
	spi_nand_cs_sel(true);

	spi_nand_rw_byte(NAND_RESET);

	spi_nand_cs_sel(false);
}

/**
 * @brief Read NAND flash ID
 */
static void spi_nand_read_id(void)
{
	spi_nand_cs_sel(true);

	// Send read ID command
	spi_nand_rw_byte(NAND_READID);

	// Send dummy byte
	spi_nand_rw_byte(DUMMY_BYTE);

	// Read 5 bytes of ID
	spi_nand_id[0] = spi_nand_rw_byte(DUMMY_BYTE);
	spi_nand_id[1] = spi_nand_rw_byte(DUMMY_BYTE);
	spi_nand_id[2] = spi_nand_rw_byte(DUMMY_BYTE);
	spi_nand_id[3] = spi_nand_rw_byte(DUMMY_BYTE);
	spi_nand_id[4] = spi_nand_rw_byte(DUMMY_BYTE);

	spi_nand_cs_sel(false);
}

/**
 * @brief Detect and configure NAND flash device
 * @return Pointer to configuration info on success, NULL on failure
 */
SPINAND_CFG_INFO_T * spinand_get_specinfo(void)
{
	uint8_t i, j;
	uint32_t spi_mode = 0;
	SPINAND_CFG_INFO_T *spinand_specinfo_ptr = NULL;

	// Try all possible SPI modes to detect the device
	for(j = 0; j < 4; j++) {
		switch(j) {
		case 0:
			spi_mode = SPI_CK_PL_LOW_PH_1EDGE;
			break;
		case 1:
			spi_mode = SPI_CK_PL_HIGH_PH_1EDGE;
			break;
		case 2:
			spi_mode = SPI_CK_PL_LOW_PH_2EDGE;
			break;
		case 3:
			spi_mode = SPI_CK_PL_HIGH_PH_2EDGE;
			break;
		default:
			break;
		}

		// Initialize SPI with current mode
		if (spi_nandflash_init(spi_mode) != 0)
			continue;

		spi_nand_reset();
		spi_nand_read_id();
		// Debug output
		printf("spi_nand_id: 0x%02x%02x%02x%02x%02x\n",
		       spi_nand_id[0], spi_nand_id[1], spi_nand_id[2],
		       spi_nand_id[3], spi_nand_id[4]);

		// Search for matching device in configuration table
		for(i = 0; i < sizeof(s_apinand_cfg_tab) / sizeof(SPINAND_CFG_INFO_T); i++) {
			// Compare first 2 bytes of device ID
			if (memcmp(s_apinand_cfg_tab[i].dev_id, spi_nand_id, 2) == 0) {
				spinand_specinfo_ptr = &s_apinand_cfg_tab[i];
				NAND_PAGE_SIZE = spinand_specinfo_ptr->pagesize;
				NAND_PAGE_PER_BLK = spinand_specinfo_ptr->page_per_blk;

				return spinand_specinfo_ptr;
			}
		}
	}
	if (spi_nand_id[0]) {
		spinand_specinfo_ptr = &s_apinand_cfg_tab[8];//4k common parameter use [11]!
		NAND_PAGE_SIZE = spinand_specinfo_ptr->pagesize;
		NAND_PAGE_PER_BLK = spinand_specinfo_ptr->page_per_blk;
		printf("no id match, use common nand driver, pg size %d, pg_per_blk %d!!\n",
		       NAND_PAGE_SIZE, NAND_PAGE_PER_BLK);

		return spinand_specinfo_ptr;
	}

	printf("nand id is 0!!\n");

	return NULL;
}

/**
 * @brief Read data from NAND flash
 * @param add Address to read from
 * @param Readbuff Buffer to store read data
 * @param Transfer_Length Number of bytes to read
 * @return NAND_OK on success, error code on failure
 */
uint16_t NANDFLASH_Read(NAND_ADDRESS add, uint8_t *Readbuff, uint16_t Transfer_Length)
{
	// Validate input parameters
	if (s_spinand_spec_info_ptr == NULL || Readbuff == NULL)
		return NAND_FAIL;

	// Enable ECC
	s_spinand_spec_info_ptr->spinnad_operation->spinand_eccenable();

	// Check if read length is valid
	if (NAND_PAGE_SIZE - add.Area < Transfer_Length)
		return NAND_FAIL;

	// Read data to cache
	s_spinand_spec_info_ptr->spinnad_operation->spinand_readtocatch(add);

	// Check ECC status
	if (s_spinand_spec_info_ptr->spinnad_operation->spinand_checkecc() == NAND_FAIL)
		return READ_NANDFLASH_ECC_ERROR;

	// Read data from cache
	s_spinand_spec_info_ptr->spinnad_operation->spinand_readfromcatch(Readbuff,
									  add, Transfer_Length);

	return NAND_OK;
}

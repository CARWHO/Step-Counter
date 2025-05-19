/*
 * imu_lsm6ds.c
 *
 *  Created on: Nov 28, 2024
 *      Author: fsy13
 */

#include "imu_lsm6ds.h"
#include "spi.h"

// Hardware configuration
#define spi_hal_handler hspi2

void imu_lsm6ds_write_byte(imu_register_t register_address, uint8_t value)
{
	uint8_t write_buff[2] = {0};
	write_buff[0] = value;
	write_buff[1] = register_address;

	// Send one word = 16 bits, MSB first
	HAL_SPI_Transmit(&spi_hal_handler, write_buff, 1, HAL_MAX_DELAY);
}

uint8_t imu_lsm6ds_read_byte(imu_register_t register_address)
{
	// 16 bit transmission:
	// First byte is the register address on MOSI, with the read bit enabled.
	// Second byte is the data from slave on MISO.
	// Indexing in reverse order due to MSB first.

	uint8_t tx[2] = {0};
	uint8_t rx[2] = {0};

	tx[1] = register_address |= (1 << 7); // Set "Read" bit

	HAL_SPI_TransmitReceive(&spi_hal_handler, tx, rx, 1, HAL_MAX_DELAY);

	return rx[0];
}

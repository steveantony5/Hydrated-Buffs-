/*
 * File : i2c_config.h
 *
 *  Created on: Apr 13, 2019
 *  Author: Sanju Prakash Kannioth
 *  Reference: https://www.silabs.com/community/blog.entry.html/2016/03/09/chapter_10_2_contro-l8qA,
 *  		   http://cache.freescale.com/files/sensors/doc/app_note/AN4070.pdf
 */

#ifndef I2C_CONFIG_H
#define I2C_CONFIG_H

#include "i2cspm.h"
#include <stdlib.h>
#include "log.h"
#include "main.h"
#include "em_i2c.h"
#include "em_core.h"
#include "em_emu.h"
#include "i2c_sensor.h"
#include "gatt_db.h"

#define ACCEL_SENSOR 0x1D << 1
#define TEMPERATURE_SENSOR_ADDR 0x80


#define CMD_ARRAY_SIZE        1
#define DATA_ARRAY_SIZE       10

uint8_t cmd_array[CMD_ARRAY_SIZE];
uint8_t data_array[DATA_ARRAY_SIZE];

int int_flag;

#define FDC1004_100HZ (0x01)
#define FDC1004_200HZ (0x02)
#define FDC1004_400HZ (0x03)
#define FDC1004_IS_RATE(x) (x == FDC1004_100HZ || \
                            x == FDC1004_200HZ || \
                            x == FDC1004_400HZ)

#define FDC1004_CAPDAC_MAX (0x1F)

#define FDC1004_CHANNEL_MAX (0x03)
#define FDC1004_IS_CHANNEL(x) (x >= 0 && x <= FDC1004_CHANNEL_MAX)

#define FDC1004_MEAS_MAX (0x03)
#define FDC1004_IS_MEAS(x) (x >= 0 && x <= FDC1004_MEAS_MAX)

#define FDC_REGISTER (0x0C)

#define ATTOFARADS_UPPER_WORD (457) //number of attofarads for each 8th most lsb (lsb of the upper 16 bit half-word)
#define FEMTOFARADS_CAPDAC (3028) //number of femtofarads for each lsb of the capdac

#define UPPER_BOUND  0X4000                 // max readout capacitance
#define LOWER_BOUND  (-1 * UPPER_BOUND)
#define CHANNEL 0                          // channel to be read
#define MEASURMENT 0                       // measurment channel

#define CAP_SENSOR ((0b01010000)<< 1)

#define DRINKING_STATE 0
#define POURING_STATE 1
#define IDLE_STATE 2

/**
--------------------------------------------------------------------------------------------
i2c_init
--------------------------------------------------------------------------------------------
*	This function works as the initialization for the I2C
*
* 	@\param			void
*
* 	@\return		void
*
**/
//int8_t i2c_init();

/**
--------------------------------------------------------------------------------------------
i2c_transfer
--------------------------------------------------------------------------------------------
*	This function works as the transfer function for the I2C
*
* 	@\param			device_address, cmd_array, data_array, data_length, cmd_length, flags
*
* 	@\return		0 on success
*
**/
int8_t i2c_transfer(uint16_t device_address, uint8_t cmd_array[], uint8_t data_array[], uint16_t data_length, uint8_t cmd_length, uint8_t flags);

/**
--------------------------------------------------------------------------------------------
i2c_read
--------------------------------------------------------------------------------------------
*	This function works as the read function for the I2C
*
* 	@\param			offset, data_len
*
* 	@\return		0 on success
* 					-1 on failure
*
**/
int8_t i2c_read(uint8_t offset, uint8_t data_len, uint8_t slave_address, uint8_t flag);


/**
--------------------------------------------------------------------------------------------
i2c_write
--------------------------------------------------------------------------------------------
*	This function works as the write function for the I2C
*
* 	@\param			offset, write_data
*
* 	@\return		0 on success
* 					-1 on failure
*
**/
int8_t i2c_write(uint8_t offset, uint16_t write_data, uint8_t slave_address, uint8_t flag,  uint8_t data_len);


int8_t accel_read();

int8_t temperature_read();

int32_t cap_read();


/**
--------------------------------------------------------------------------------------------
accel_config_freefall
--------------------------------------------------------------------------------------------
*	This function works as the freefall configuration for MMA8452Q Accelerometer
*
* 	@\param			void
*
* 	@\return		0 on success
* 					-1 on failure
*
**/
int8_t accel_config_freefall();


/**
--------------------------------------------------------------------------------------------
accel_config_tap
--------------------------------------------------------------------------------------------
*	This function works as the tap configuration for MMA8452Q Accelerometer
*
* 	@\param			void
*
* 	@\return		0 on success
* 					-1 on failure
*
**/
int8_t accel_config_tap();
#endif

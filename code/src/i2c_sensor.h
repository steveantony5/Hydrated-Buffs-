/**
 * @\file	i2c_sensor.h
 * @\author	Sorabh Gandhi
 * @\brief	This file contains declaration of functions to enable, read and disable the temperature sensor via i2c
 * @\date	12/01/2019
 *
 */

#ifndef SRC_I2C_SENSOR_H_
#define SRC_I2C_SENSOR_H_

/*SDK headers*/
#include "i2cspm.h"
#include "log.h"

#include "em_i2c.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_core.h"
#include "bsp.h"

/*own headers*/
#include "scheduler.h"
#include "main.h"


/**------------------------------------------------------------------------------------------------
MACRO DEFINITIONS
--------------------------------------------------------------------------------------------------*/
#define SI7021_ADDRESS 0x80				//I2C Sensor Address
#define SI7021_CMD_MEASURE_TEMP 0xE3	//Write Command to read temperature

/*******************************************************************************
*				EXPORTED FUNCTIONS
*******************************************************************************/

/**
--------------------------------------------------------------------------------------------------
i2c_init
--------------------------------------------------------------------------------------------------
*	This function will initialize and enable I2C and also enables the temperature sensor - SI7021
*
* 	@\param				none
*
* 	@\return			void
*/
void i2c_init();

/**
--------------------------------------------------------------------------------------------------
get_temperature
--------------------------------------------------------------------------------------------------
*	This function will convert the raw sensor value into temperature data in deg C and then notify
*	the attribute to the connected client
* 	@\param i2c			none
*
* 	@\return			void
*/
void get_temperature();

/**
--------------------------------------------------------------------------------------------------
i2c_deinit
--------------------------------------------------------------------------------------------------
*	This function will disable the temperature sensor - SI7021
*
* 	@\param				none
*
* 	@\return			void
*/
void i2c_deinit();

/**
--------------------------------------------------------------------------------------------------
i2c_read_command
--------------------------------------------------------------------------------------------------
*	This function reads the output from temperature sensor (SI7021) via internal I2C bus
*
* 	@\param				none
*
* 	@\return			void
*/
void i2c_read_command();


#endif /* SRC_I2C_SENSOR_H_ */

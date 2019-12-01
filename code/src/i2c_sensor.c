/**
 * @\file	i2c_sensor.c
 * @\author	Sorabh Gandhi
 * @\brief	This file contains definition of functions to enable, read and disable the temperature sensor via i2c
 * @\date	02/06/2019
 *
 */

/* Own headers */
#include <src/i2c_sensor.h>

uint8_t i2c_read_data[2];
uint8_t i2c_write_data[1];
I2C_TransferSeq_TypeDef seq;
volatile I2C_TransferReturn_TypeDef ret;

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
void i2c_init()
{
	I2CSPM_Init_TypeDef i2cInit = I2CSPM_INIT_DEFAULT;
	I2CSPM_Init(&i2cInit);

	seq.addr  = SI7021_ADDRESS;
	seq.flags = I2C_FLAG_WRITE;

	i2c_write_data[0] = SI7021_CMD_MEASURE_TEMP;
	seq.buf[0].data   = i2c_write_data;
	seq.buf[0].len    = 2;

	NVIC_EnableIRQ(I2C0_IRQn);
	ret = I2C_TransferInit(I2C0, &seq);

	LOG_INFO("Return Status for write = %d", ret);
}

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
void i2c_read_command()
{
	seq.flags = I2C_FLAG_READ;
	seq.buf[0].data = i2c_read_data;
	seq.buf[0].len = 2;

	ret = I2C_TransferInit(I2C0, &seq);

	LOG_INFO("Return Status for read = %d", ret);
}

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
void get_temperature()
{
	int32_t data;
	uint32_t temperature;
	uint8_t databuffer[5];
	uint8_t *p = databuffer;
	uint8_t flag = 0x00;

	data = (((uint32_t)i2c_read_data[0] << 8) | i2c_read_data[1]);
	data = (((175.72 * data) / 65536) - 46.85);	//Covert the sensor readings into temperature data in deg C)

	LOG_INFO("Temp = %d\n", data);

	UINT8_TO_BITSTREAM(p, flag);
	temperature = FLT_TO_UINT32((data * 1000), -3);
	UINT32_TO_BITSTREAM(p, temperature);

	BTSTACK_CHECK_RESPONSE(gecko_cmd_gatt_server_send_characteristic_notification(0xFF, gattdb_temperature_measurement, 5, databuffer));
	BTSTACK_CHECK_RESPONSE(gecko_cmd_le_connection_get_rssi(connection_handle));

//	GPIO_PinOutClear(gpioPortD, 15);
}

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
void i2c_deinit()
{
	I2C_Enable(I2C0, false);
}

/*Defining the I2C based IRQ event*/
void I2C0_IRQHandler(void)
{
	CORE_ATOMIC_IRQ_DISABLE();

	ret = I2C_Transfer(I2C0);

	if (ret == i2cTransferDone) {
		events.TX_DONE = true;
	}
	else if (ret == i2cTransferInProgress) {
		events.TX_ERROR = true;
	}
	else {

	}

	LOG_INFO("Interrupt - %d", ret);

	CORE_ATOMIC_IRQ_ENABLE();
	gecko_external_signal(events.TX_DONE);
}

/*
 * File : i2c_config.c
 *
 *  Created on: Apr 13, 2019
 *  Author: Sanju Prakash Kannioth
 *  Reference: https://www.silabs.com/community/blog.entry.html/2016/03/09/chapter_10_2_contro-l8qA,
 *  		   http://cache.freescale.com/files/sensors/doc/app_note/AN4070.pdf
 */
#include "i2c_config.h"


int timeout = 50000;

uint8_t MEAS_CONFIG[] = {0x08, 0x09, 0x0A, 0x0B};
uint8_t MEAS_MSB[] = {0x00, 0x02, 0x04, 0x06};
uint8_t MEAS_LSB[] = {0x01, 0x03, 0x05, 0x07};
uint8_t SAMPLE_DELAY[] = {11,11,6,3};

I2C_TransferReturn_TypeDef result;

static void temp_calculation()
{
	int32_t data;
	data = ((175.72*((uint32_t)data_array[0]<<8) + data_array[1])/65535) - 46.85; // Calculation to calculate temperature in degree C
	LOG_INFO("Temp value = %d",data);
//	if(data >= 23) {
//		gpioLed1SetOn();
//		gpioLed0SetOff();
//
//	}
//	else if(data <= 22){
//		gpioLed0SetOn();
//		gpioLed1SetOff();
//	}
//	else {
//		gpioLed0SetOff();
//		gpioLed1SetOff();
//	}
}

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

//int8_t i2c_init()
//{
//	I2CSPM_Init_TypeDef i2cInit = I2CSPM_INIT_DEFAULT;
//	I2CSPM_Init(&i2cInit); // Initialize I2C
////	I2C_Enable(I2C0, 0); // Disabling I2C0
//	LOG_INFO("Init Complete\n");
//
//	return 0;
//}

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
int8_t i2c_transfer(uint16_t device_address, uint8_t cmd_array[], uint8_t data_array[], uint16_t data_length, uint8_t cmd_length, uint8_t flags)
{

	I2C_TransferSeq_TypeDef i2cTransfer;
	if(flags == I2C_FLAG_WRITE_WRITE || flags == I2C_FLAG_WRITE_READ)
	{
		i2cTransfer.addr          = device_address;
		i2cTransfer.flags         = flags;
		i2cTransfer.buf[0].data   = cmd_array;
		i2cTransfer.buf[0].len    = cmd_length;

		i2cTransfer.buf[1].data   = data_array;
		i2cTransfer.buf[1].len    = data_length;

	}

	else if(flags == I2C_FLAG_WRITE)
	{
		i2cTransfer.addr  = device_address;
		i2cTransfer.flags = flags; // Write flag set

		i2cTransfer.buf[0].data   = cmd_array;
		i2cTransfer.buf[0].len    = cmd_length;
	}

	else if(flags == I2C_FLAG_READ)
	{
		i2cTransfer.addr  = device_address;
		i2cTransfer.flags = flags; // Read flag set

		i2cTransfer.buf[0].data   = data_array;
		i2cTransfer.buf[0].len    = data_length;
	}

	LOG_INFO("Data length = %x", device_address);
	NVIC_EnableIRQ(I2C0_IRQn);

	result = I2C_TransferInit(I2C0, &i2cTransfer);
//	result = I2CSPM_Transfer(I2C0, &i2cTransfer);

//	if(result != i2cTransferDone)
//	{
//		LOG_ERROR("Error");
//		return 2;
//	}

	while (!events.TX_DONE && timeout != 0)
	{
		timeout--;
	}
//
//
////	LOG_INFO("SLEEP MODE : %d\n",SLEEP_LowestEnergyModeGet());
//
	CORE_ATOMIC_IRQ_DISABLE();
	events.TX_DONE = 0;
	CORE_ATOMIC_IRQ_ENABLE();

	NVIC_DisableIRQ(I2C0_IRQn);

	return 0;
}

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
int8_t i2c_read(uint8_t offset, uint8_t data_len, uint8_t slave_address, uint8_t flag)
{
	cmd_array[0] = offset;
	if(i2c_transfer(slave_address, cmd_array, data_array, data_len, 1, flag) == 0)
	{
//		LOG_DEBUG("I2C Read transfer success\n");
		return 0;
	}

	else
	{
		LOG_ERROR("Failed I2C Read\n");
		return -1;
	}


}

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
int8_t i2c_write(uint8_t offset, uint16_t write_data, uint8_t slave_address, uint8_t flag,  uint8_t data_len)
{
	cmd_array[0] = offset;
	data_array[0] = write_data &0x0F;

//	LOG_INFO("Writing data %x", data_array[0]);
	if(data_len == 2 && (flag == I2C_FLAG_WRITE_WRITE || flag == I2C_FLAG_WRITE)) {
		data_array[0] = (uint8_t)(write_data >> 8);
		data_array[1] = (uint8_t)write_data;
	}

	if(i2c_transfer(slave_address, cmd_array, data_array, data_len, 1, flag) == 0)
	{
//			LOG_DEBUG("I2C Write transfer success\n");
			return 0;
	}

	else
	{
		LOG_ERROR("Failed I2C Write\n");
		return -1;
	}
}


void configureMeasurementSingle(uint8_t measurement, uint8_t channel, uint8_t capdac) {
	uint16_t configuration_data;
	configuration_data = ((uint16_t)channel) << 13; //CHA
	configuration_data |=  ((uint16_t)0x04) << 10; //CHB disable / CAPDAC enable
	configuration_data |= ((uint16_t)capdac) << 5; //CAPDAC value

	if(i2c_write(MEAS_CONFIG[measurement], configuration_data, CAP_SENSOR, I2C_FLAG_WRITE_WRITE, 2) != 0)
	{
//		LOG_DEBUG("I2C Write transfer success\n");
	}

	LOG_INFO("Measurement configuration completed");
}


void triggerSingleMeasurement(uint8_t measurement, uint8_t rate) {
	uint16_t trigger_data;
	trigger_data = ((uint16_t)rate) << 10; // sample rate
	trigger_data |= 0 << 8; //repeat disabled
	trigger_data |= (1 << (7-measurement)); // 0 > bit 7, 1 > bit 6, etc

	if(i2c_write(FDC_REGISTER, trigger_data, CAP_SENSOR, I2C_FLAG_WRITE_WRITE, 2) != 0)
	{
//		LOG_DEBUG("I2C Write transfer success\n");
	}

	LOG_INFO("Measurement trigger completed");
}

void readMeasurement(uint8_t measurement, uint16_t * value) {
	 i2c_read(FDC_REGISTER, 2, CAP_SENSOR, I2C_FLAG_WRITE_READ);
	 uint16_t fdc_register = data_array[0];
	 fdc_register <<= 8;
	 fdc_register |= data_array[1];

//	 LOG_INFO("FDC Reg value = %x", fdc_register);
	 if (! (fdc_register & ( 1 << (3-measurement)))) {
	    LOG_INFO("measurement not completed");
	 }

	  //read the value

	  i2c_read(MEAS_MSB[measurement], 2, CAP_SENSOR, I2C_FLAG_WRITE_READ);

	  uint16_t msb = data_array[0];
	  msb <<= 8;
	  msb |= data_array[1];

	  i2c_read(MEAS_LSB[measurement], 2, CAP_SENSOR, I2C_FLAG_WRITE_READ);
	  uint16_t lsb = data_array[0];
	  lsb <<= 8;
	  lsb |= data_array[1];

	  value[0] = msb;
	  value[1] = lsb;
}

int32_t cap_read() {
	int capdac = 0;
	configureMeasurementSingle(MEASURMENT, CHANNEL, capdac);
	triggerSingleMeasurement(MEASURMENT, FDC1004_200HZ);

	uint16_t value[2];
	readMeasurement(MEASURMENT, value);

	int16_t msb = (int16_t) value[0];
	int32_t capacitance = ((int32_t)457) * ((int32_t)msb); //in attofarads
	capacitance /= 1000;   //in femtofarads
	capacitance += ((int32_t)3028) * ((int32_t)capdac);
	LOG_INFO("cap %d\n", capacitance);


//	uint8_t databuffer[5];
//	uint8_t *p = databuffer;
//	uint8_t flag = 0x00;
//	uint32_t temp_cap;
//
//	//UINT8_TO_BITSTREAM(p, flag);
//	//temp_cap = FLT_TO_UINT32((capacitance * 1000), -3);
//	//UINT32_TO_BITSTREAM(p, capacitance);
//	itoa(capacitance, databuffer, 10);   // here 10 means decimal
//	LOG_INFO("Decimal value = %s\n", databuffer);
//
//	BTSTACK_CHECK_RESPONSE(gecko_cmd_gatt_server_send_characteristic_notification(0xFF, gattdb_water_level_value, 5, databuffer));

	return capacitance;
}


/* https://github.com/ControlEverythingCommunity/MMA8452Q/blob/master/C/MMA8452Q.c */
int8_t accel_read() {
//	data_array[0] = 0;
	if(i2c_write(0x2A, 0x20, ACCEL_SENSOR, I2C_FLAG_WRITE_WRITE, 1) != 0)
	{
	  LOG_ERROR("Failed in putting device in standby mode\n");
	  return -1;
	}

	if(i2c_read(0x2A, 1, ACCEL_SENSOR, I2C_FLAG_WRITE_READ) != 0)
	{
	  LOG_ERROR("Failed in Read\n");
	  return -1;
	}

	int temp = data_array[0];
	temp &= ~(0x01);

	LOG_INFO("CNTRL REG = %x", temp);

	// Put device in active mode
	if(i2c_write(0x2A, 0x01, ACCEL_SENSOR, I2C_FLAG_WRITE_WRITE, 1) != 0)
	{
	  LOG_ERROR("Failed in putting device in active mode\n");
	}

	if(i2c_write(0x0E, 0x00, ACCEL_SENSOR, I2C_FLAG_WRITE_WRITE, 1) != 0)
	{
		LOG_ERROR("Failed in Read\n");
	}

	if(i2c_read(0x0E, 1, ACCEL_SENSOR, I2C_FLAG_WRITE_READ) != 0)
	{
		LOG_ERROR("Failed in Read\n");
	}

//	if(i2c_write(0x0E, 0x00, ACCEL_SENSOR, I2C_FLAG_WRITE_WRITE, 1) != 0)
//	{
//		LOG_ERROR("Failed in Read\n");
//	}
//
//	if(i2c_read(0x0E, 1, ACCEL_SENSOR, I2C_FLAG_WRITE_READ) != 0)
//	{
//		LOG_ERROR("Failed in Read\n");
//	}

	LOG_INFO("Check G = %x", data_array[0]);


	if(i2c_read(0x01, 6, ACCEL_SENSOR, I2C_FLAG_WRITE_READ) != 0)
	{
	  LOG_ERROR("Failed in Read\n");
	  return -1;
	}

	int x = ((short)(data_array[0] << 8 | data_array[1])) >> 4;
	int y = ((short)(data_array[2] << 8 | data_array[3])) >> 4;
	int z = ((short)(data_array[4] << 8 | data_array[5])) >> 4;

//	int cx = (float)x / (float)(1 << 11) * (float)(2);
//	int cy = (float)y / (float)(1 << 11) * (float)(2);
//	int cz = (float)z / (float)(1 << 11) * (float)(2);

	LOG_INFO("X = %d\n", x);
	LOG_INFO("Y = %d\n", y);
	LOG_INFO("Z = %d\n", z);

	if(((y > 200 || y < -200) || (x > 200 || x < -200)) && z > -900) {
		LOG_INFO("DRINKING");
		char value[] = "Drinking";
		gecko_cmd_gatt_server_send_characteristic_notification(0xFF, gattdb_accel_value_level, 8, &value);
		gpioLed1SetOn();
		gpioLed0SetOff();
		return DRINKING_STATE;
	}
	else if(z < -900) {
		LOG_INFO("POURING");
		char value[] = "Pouring";
		gecko_cmd_gatt_server_send_characteristic_notification(0xFF, gattdb_accel_value_level, 7, &value);
		gpioLed0SetOn();
		gpioLed1SetOff();

		return POURING_STATE;
	}
	else {
		gpioLed0SetOff();
		gpioLed1SetOff();
		char value[] = "Idle";
		gecko_cmd_gatt_server_send_characteristic_notification(0xFF, gattdb_accel_value_level, 4, &value);
		return IDLE_STATE;
	}

	return 0;
}

/**
--------------------------------------------------------------------------------------------
I2C0_IRQHandler
--------------------------------------------------------------------------------------------
*	This function works as the I2C0 IRQ Handler
*
* 	@\param			void
*
* 	@\return		void
*
**/
//void I2C0_IRQHandler(void)
//{
//	CORE_ATOMIC_IRQ_DISABLE();
//
//	result = I2C_Transfer(I2C0);
//
//	if(result == i2cTransferDone)
//	{
//		LOG_INFO("Int transfer done flag set");
//		sch_event.tx_done = 1; // Set I2C0 transfer complete event
//	}
//	else if(result != i2cTransferInProgress)
//	{
//		sch_event.tx_err = 1; // Set I2C0 transfer error event
//	}
//
//	CORE_ATOMIC_IRQ_ENABLE();
//}

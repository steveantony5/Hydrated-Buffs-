/*
 * scheduler.c
 *
 *  Created on: 06-Feb-2019
 *      Author: Sorabh
 */

/*Own Headers*/
#include "scheduler.h"
#include "my_sleep.h"

/**
--------------------------------------------------------------------------------------------------
event_handler
--------------------------------------------------------------------------------------------------
*	This function will run a sequence of events to enable sensor, fetch the temperature and
*	then disable sensor
*
* 	@\param				none
*
* 	@\return			void
*/
void event_handler()
{
	uint32_t data;

	I2C_Enable(I2C0, true);	//Enable the I2C bus

	//timerWaitUs(80000);	//Wait for 80ms

	get_temperature(I2C0, SI7021_ADDRESS, SI7021_CMD_MEASURE_TEMP, &data);	//get the temperature

	//i2c_deinit();	//Disable the I2C bus and sensor

	data = (((175.72 * data) / 65536) - 46.85);	//Covert the sensor readings into temperature data in deg C)

	LOG_INFO("Temp = %d\n", data);

	sch_event = false;
}

/**
--------------------------------------------------------------------------------------------------
state_machine
--------------------------------------------------------------------------------------------------
*	This is a custom state machine to initialize, write, read and process the temperature sensor value
*	via I2C bus
*
* 	@\param	signals		This is an external signal which is notified by gecko_external_signal function
*
* 	@\return			void
*/
void state_machine(uint32 signals)
{
	CORE_DECLARE_IRQ_STATE;
	switch (CURRENT_STATE)
	{
		uint32_t data = 0;
		case POWER_OFF_STATE:
		{
			if (events.COMP0_INTERRUPT & signals) {
				CORE_ENTER_CRITICAL();
				events.COMP0_INTERRUPT = false;
				events.NO_EVENT = true;
				CORE_EXIT_CRITICAL();

				//sensor on
				I2C_Enable(I2C0, true);
				GPIO_PinOutSet(gpioPortD, 15);

				//timer_80 ms
				timerWaitUs(80);
				CURRENT_STATE = WAIT_FOR_POWER_ON_STATE;
			}

			break;
		}

		case WAIT_FOR_POWER_ON_STATE:
		{
			if (events.COMP1_INTERRUPT & signals) {
				CORE_ENTER_CRITICAL();
				events.COMP1_INTERRUPT = false;
				events.NO_EVENT = true;
				CORE_EXIT_CRITICAL();

				i2c_init();
				CURRENT_STATE = WAIT_FOR_I2C_COMMAND_WRITE_STATE;
		  	}

			break;
		}

		case WAIT_FOR_I2C_COMMAND_WRITE_STATE:
		{
			if (events.TX_DONE & signals) {
				i2c_read_command();
				CURRENT_STATE = WAIT_FOR_I2C_TEMPERATURE_READ_STATE;
			}

		  	break;
		}

		case WAIT_FOR_I2C_TEMPERATURE_READ_STATE:
		{
			if (events.TX_DONE & signals) {
				CORE_ENTER_CRITICAL();
				events.TX_DONE = false;
				sch_event = false;
				events.NO_EVENT = true;
				CORE_EXIT_CRITICAL();

				get_temperature();
		  		CURRENT_STATE = POWER_OFF_STATE;
		  	}

		  	 break;
		}

	}
}

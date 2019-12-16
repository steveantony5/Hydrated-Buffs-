/*
 * scheduler.h
 *
 *  Created on: 06-Feb-2019
 *      Author: Sorabh
 */

#ifndef SRC_SCHEDULER_H_
#define SRC_SCHEDULER_H_

#include <stdbool.h>

/*SDK header*/
#include "i2c_sensor.h"
#include "timer.h"

/*variable to notify the event status*/
bool sch_event;

/*******************************************************************************
*  				STRUCTURES
*******************************************************************************/
typedef enum {
	POWER_OFF_STATE = 0,
	WAIT_FOR_POWER_ON_STATE,
	WAIT_FOR_I2C_COMMAND_WRITE_STATE,
	WAIT_FOR_I2C_TEMPERATURE_READ_STATE,
} state;

state CURRENT_STATE;

typedef struct event {
	bool COMP0_INTERRUPT;
	bool COMP1_INTERRUPT;
	bool TX_DONE;
	bool TX_ERROR;
	bool NO_EVENT;
} event;

volatile event events;

/*******************************************************************************
*				EXPORTED FUNCTIONS
*******************************************************************************/

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
void event_handler();

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
void state_machine(uint32 signals);

#endif /* SRC_SCHEDULER_H_ */

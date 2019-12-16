/**
 * @\file	timer.h
 * @\author	Sorabh Gandhi
 * @\brief	This file contains declaration for timer initialization and delay function
 * @\date	02/06/2019
 *
 */

#ifndef SRC_TIMER_H_
#define SRC_TIMER_H_

/*System header*/
#include <math.h>

/*SDK header*/
#include "em_core.h"
#include "em_letimer.h"
#include "em_cmu.h"
#include "cmu.h"

/*Own header*/
#include "gpio.h"
#include "main.h"
#include "i2c_sensor.h"
#include "scheduler.h"

/*******************************************************************************
*				DEFINITIONS AND MACROS
*******************************************************************************/

/* Define the Ultra low frequency used by EM3 and EM4 */
#define ULTRA_LOW_FREQUENCY (1000)

/* Define the Ultra low frequency used by EM0, EM1 and EM2 */
#define LOW_FREQUENCY (32768)

/* Max count = (2^16) */
#define MAX_COUNT (65535)

/*******************************************************************************
*				EXPORTED VARIABLES
*******************************************************************************/

uint32_t ref_freq;
uint32_t osc_freq;
uint32_t rollover_counter;


/*******************************************************************************
*				EXPORTED FUNCTIONS
*******************************************************************************/

void timer_Init(void);
void calculate_prescaler(uint32_t osc_freq);

/**
--------------------------------------------------------------------------------------------------
timerWaitUs
--------------------------------------------------------------------------------------------------
*	This function will create a delay for specified period passed in the argument
*
* 	@\param				delay period in microseconds
*
* 	@\return			void
*/
void timerWaitUs(uint32_t us_wait);

#endif /* SRC_TIMER_H_ */

//Reference: Inherited from Dan's logging module

/**
 * @\file	log.c
 * @\author	Sorabh Gandhi / Sanju Prakash Kannioth / Steve
 * @\brief	This header file contains the definition for all the log functionalities
 * @\date	12/25/2019
 *
 */

#include "retargetserial.h"
#include "log.h"
#include "timer.h"
#include <stdbool.h>

#if INCLUDE_LOGGING
/**
 * @return a timestamp value for the logger, typically based on a free running timer.
 * This will be printed at the beginning of each log message.
 */
uint32_t loggerGetTimestamp(void)
{
	uint32_t total_time;
		total_time = ((rollover_counter * 3) + (LETIMER_CounterGet(LETIMER0) * (2 / 32678)));
		return total_time;
	//return timerGetRunTimeMilliseconds();
}
/*
uint32_t timerGetRunTimeMilliseconds(void)
{
	uint32_t total_time;
	total_time = ((rollover_counter * 3) + (LETIMER_CounterGet(LETIMER0) * (2 / 32678)));
	return total_time;
}*/

/**
 * Initialize logging for Blue Gecko.
 * See https://www.silabs.com/community/wireless/bluetooth/forum.topic.html/how_to_do_uart_loggi-ByI
 */
void logInit(void)
{
	RETARGET_SerialInit();
	/**
	 * See https://siliconlabs.github.io/Gecko_SDK_Doc/efm32g/html/group__RetargetIo.html#ga9e36c68713259dd181ef349430ba0096
	 * RETARGET_SerialCrLf() ensures each linefeed also includes carriage return.  Without it, the first character is shifted in TeraTerm
	 */
	RETARGET_SerialCrLf(true);
	LOG_INFO("Initialized Logging");
}

/**
 * Block for chars to be flushed out of the serial port.  Important to do this before entering SLEEP() or you may see garbage chars output.
 */
void logFlush(void)
{
	RETARGET_SerialFlush();
}
#endif

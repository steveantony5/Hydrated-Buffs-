/**
 * @\file	cmu.c
 * @\author	Sorabh Gandhi / Sanju Prakash Kannioth / Steve
 * @\brief	This header file contains the definition of all 
 *			functionalities used to initialize high frequency 
 *			low frequency crystal oscillator
 * @\date	12/25/2019
 *
 */

#include "cmu.h"

void cmu_init()
{
	/*CMU configuration for Ultra Low Power mode*/
	if ((ENERGY_MODE == 3) || (ENERGY_MODE == 4)) {

		//Enable Oscillator for defined Energy Mode
		CMU_OscillatorEnable(cmuOsc_ULFRCO, true, true);

		//Set Oscillator for the peripheral clock
		CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);
	}

	/*CMU configuration for Low Power mode*/
	if ((ENERGY_MODE == 0) || (ENERGY_MODE == 1) || (ENERGY_MODE == 2)) {

		//Enable Oscillator for defined Energy Mode
		CMU_OscillatorEnable(cmuOsc_LFXO, true, true);

		//Set Oscillator for the peripheral clock
		CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
	}

	CMU_ClockEnable(cmuClock_LFA, true);

	//Enable clock for LETIMER0
	CMU_ClockEnable(cmuClock_LETIMER0, true);

}








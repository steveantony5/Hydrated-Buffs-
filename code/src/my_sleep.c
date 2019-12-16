/**
 * @\file	my_sleep.c
 * @\author	Sorabh Gandhi / Sanju Prakash Kannioth / Steve
 * @\brief	This header file contains the definition of all 
 *			functionalities to switch in different energy modes
 * @\date	12/25/2019
 *
 */

#include "my_sleep.h"

//Initialize sleep modes for all the energy modes
void sleep_configure()
{
	const SLEEP_Init_t init = { 0 };
	SLEEP_InitEx(&init);
}

//Set an enter the appropriate energy mode
void sleep()
{
	switch (ENERGY_MODE)
	{

	case 0:
		break;

	case 1:
		EMU_EnterEM1();
		break;

	case 2:
		EMU_EnterEM2(1);
		break;

	case 3:
		EMU_EnterEM3(1);
		break;

	case 4:
		EMU_EnterEM3(1);
		break;

	}
}

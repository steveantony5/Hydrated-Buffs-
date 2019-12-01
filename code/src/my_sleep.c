/*
 * my_sleep.c
 *
 *  Created on: 29-Jan-2019
 *      Author: Sorabh
 */

#include "my_sleep.h"

void sleep_configure()
{
	const SLEEP_Init_t init = { 0 };
	SLEEP_InitEx(&init);
}

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

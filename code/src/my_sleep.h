/**
 * @\file	my_sleep.h
 * @\author	Sorabh Gandhi / Sanju Prakash Kannioth / Steve
 * @\brief	This header file contains the declaration of all 
 *			functionalities to switch in different energy modes
 * @\date	12/25/2019
 *
 */

#ifndef SRC_MY_SLEEP_H_
#define SRC_MY_SLEEP_H_

#include "sleep.h"
#include "em_emu.h"
#include "em_core.h"
#include "main.h"

void sleep_configure();
void sleep();

#endif /* SRC_MY_SLEEP_H_ */

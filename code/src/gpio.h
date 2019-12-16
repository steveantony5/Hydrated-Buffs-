/**
 * @\file	gpio.h
 * @\author	Sorabh Gandhi / Sanju Prakash Kannioth / Steve
 * @\brief	This header file contains the declaration of all 
 *			functionalities used to onboard push button and led
 * @\date	12/25/2019
 *
 */

#ifndef SRC_GPIO_H_
#define SRC_GPIO_H_
#include <stdbool.h>
#include <stdint.h>

//onboard push button
#define PB0_BUTTON_PORT gpioPortF
#define PB0_BUTTON_PIN 6

//onboard push button on development board
#define PB1_BUTTON_PORT gpioPortF
#define PB1_BUTTON_PIN 7		1

#define EXT_SIGNAL_PB0_BUTTON_PRESSED 0x01
#define EXT_SIGNAL_PB0_BUTTON_RELEASED 0x02
#define BUTTON_STATUS 0x20

volatile uint8_t EXT_SIGNAL_PB0_BUTTON;

void gpioInit();
void gpioLed0SetOn();
void gpioLed0SetOff();
void gpioLed1SetOn();
void gpioLed1SetOff();

void gpioEnableDisplay();
void gpioSetDisplayExtcomin(bool high);

void enable_button_interrupts(void);
void gpioint(uint8_t pin);

#endif /* SRC_GPIO_H_ */

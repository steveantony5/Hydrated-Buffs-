/**
 * @\file	gpio.c
 * @\author	Sorabh Gandhi / Sanju Prakash Kannioth / Steve
 * @\brief	This header file contains the definition of all 
 *			functionalities used to onboard push button and led
 * @\date	12/25/2019
 *
 */
 
#include "gpio.h"
#include "em_gpio.h"
#include <string.h>
#include "em_core.h"


#define	LED0_port gpioPortC
#define LED0_pin	6
#define LED1_port gpioPortF
#define LED1_pin 5

void gpioInit()
{
	GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthWeakAlternateStrong);
	//GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(LED0_port, LED0_pin, gpioModePushPull, false);
	GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthWeakAlternateStrong);
	//GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(LED1_port, LED1_pin, gpioModePushPull, false);

	GPIO_PinModeSet(PB0_BUTTON_PORT, PB0_BUTTON_PIN, gpioModeInputPull, true);
	GPIO_PinModeSet(PB1_BUTTON_PORT, PB1_BUTTON_PIN, gpioModeInputPull, true);
}

void gpioLed0SetOn()
{
	GPIO_PinOutSet(LED0_port,LED0_pin);
}
void gpioLed0SetOff()
{
	GPIO_PinOutClear(LED0_port,LED0_pin);
}
void gpioLed1SetOn()
{
	GPIO_PinOutSet(LED1_port,LED1_pin);
}
void gpioLed1SetOff()
{
	GPIO_PinOutClear(LED1_port,LED1_pin);
}

void gpioEnableDisplay()
{
	GPIO_PinOutSet(gpioPortD, 15);
}

void gpioSetDisplayExtcomin(bool high)
{
	if (high) {
		GPIO_PinOutSet(gpioPortD, 13);
	} else {
		GPIO_PinOutClear(gpioPortD, 13);
	}
}

void gpioint(uint8_t pin)
{
	if (pin == PB0_BUTTON_PIN)
	{
		NVIC_DisableIRQ(GPIO_ODD_IRQn);
		NVIC_DisableIRQ(GPIO_EVEN_IRQn);

		EXT_SIGNAL_PB0_BUTTON = 1;
		gecko_external_signal(EXT_SIGNAL_PB0_BUTTON);
	}
}


void enable_button_interrupts(void)
{
  GPIOINT_Init();

  /* configure interrupt for PB0 and PB1, both falling and rising edges */
  GPIO_ExtIntConfig(PB0_BUTTON_PORT, PB0_BUTTON_PIN, PB0_BUTTON_PIN, true, false, true);

  /* register the callback function that is invoked when interrupt occurs */
  GPIOINT_CallbackRegister(PB0_BUTTON_PIN, gpioint);
}

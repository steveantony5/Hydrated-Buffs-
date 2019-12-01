/**
 * @\file	timer.c
 * @\author	Sorabh Gandhi
 * @\brief	This file contains definition for timer initialization function and delay function
 * @\date	02/06/2019
 *
 */

/*Own SDK*/
#include "timer.h"

//rollover_counter = 0;

void timer_Init()
{


	if ((ENERGY_MODE == 3) || (ENERGY_MODE == 4)) {
		osc_freq = ULTRA_LOW_FREQUENCY;
	}

	if ((ENERGY_MODE == 0) || (ENERGY_MODE == 1) || (ENERGY_MODE == 2)) {
		osc_freq = LOW_FREQUENCY;
	}

	const LETIMER_Init_TypeDef letimerInit =
	{
		.enable			= false,                  /* Disable the counter */
		.debugRun		= false,                  /* Counter shall not keep running during debug halt. */
		.comp0Top		= true,                   /* Load COMP0 register into CNT when counter underflows. COMP0 is used as TOP */
		.bufTop			= false,                  /* Don't load COMP1 into COMP0 when REP0 reaches 0. */
		.out0Pol		= 0,                      /* Idle value for output 0. */
		.out1Pol		= 0,                      /* Idle value for output 1. */
		.ufoa0			= letimerUFOANone,
		.ufoa1			= letimerUFOANone,
		.repMode		= letimerRepeatFree
	};

	//Initialize LETIMER
	LETIMER_Init(LETIMER0, &letimerInit);

	while (LETIMER0->SYNCBUSY != 0);

	//Enable COMP1 and UnderFlow Interrupt
	LETIMER_IntEnable(LETIMER0, (LETIMER_IEN_UF));

	//Enable NVIC Interrupt
	NVIC_EnableIRQ(LETIMER0_IRQn);

	if (osc_freq > 0) {

		/* Calculate the frequency depending on mode*/
		calculate_prescaler(osc_freq);
	}

	LETIMER_IntClear(LETIMER0, (LETIMER_IEN_UF));

	LETIMER_Enable(LETIMER0, true);

	LETIMER0->CMD = LETIMER_CMD_START;
}

void calculate_prescaler(uint32_t osc_freq)
{
	uint32_t prescaler, comp0_value, comp1_value, power;

	if (PERIOD >= 2)
	{
		power = ((PERIOD * osc_freq) / (MAX_COUNT));

		prescaler = pow(2, power);

		CMU_ClockDivSet(cmuClock_LETIMER0, prescaler); //Prescaling Frequency

		ref_freq = (osc_freq / prescaler);

		comp0_value = (ref_freq * PERIOD);

		comp1_value = (ref_freq * (PERIOD - LED_ON_TIME));
	}
	else {

		comp0_value = (osc_freq * PERIOD);

		comp1_value = (osc_freq * (PERIOD - LED_ON_TIME));
	}


	LETIMER_CompareSet(LETIMER0, 0, comp0_value);
	while (LETIMER0->SYNCBUSY != 0);

	LETIMER_CompareSet(LETIMER0, 1, comp1_value);
	while (LETIMER0->SYNCBUSY != 0);
}

void LETIMER0_IRQHandler (void)
{
	//Disable interrupt
	CORE_ATOMIC_IRQ_DISABLE();

	if (LETIMER_IntGet(LETIMER0) & LETIMER_IF_COMP0)
	{
		LETIMER_IntClear(LETIMER0, LETIMER_IEN_UF);
		events.COMP0_INTERRUPT = true;
		rollover_counter++;
	}

	//Check interrupt due to UF
	if ((LETIMER_IntGet(LETIMER0) & LETIMER_IF_COMP1))
	{
		//Clear all interrupts due to UF
		LETIMER_IntClear(LETIMER0, LETIMER_IF_COMP1);

		LETIMER_IntDisable(LETIMER0, (LETIMER_IF_COMP1));

		events.COMP1_INTERRUPT = true;

		//set a scheduler event
		sch_event = true;
	}

	//Enable interrupt
	CORE_ATOMIC_IRQ_ENABLE();
	gecko_external_signal(events.COMP0_INTERRUPT);
	gecko_external_signal(events.COMP1_INTERRUPT);
}

void timerWaitUs(uint32_t us_wait)
{
	CORE_ATOMIC_IRQ_DISABLE();
	uint32_t ticks = ((ref_freq * us_wait) / 1000);
	uint32_t value;
	if (LETIMER_CounterGet(LETIMER0) > ticks) {
		value = (LETIMER_CounterGet(LETIMER0) - ticks);
	} else {
		value = ((osc_freq * PERIOD) - ((ticks) -  LETIMER_CounterGet(LETIMER0)));
	}

	LETIMER_CompareSet(LETIMER0, 1, value);
	LETIMER_IntClear(LETIMER0, LETIMER_IF_COMP1);
	LETIMER_IntEnable(LETIMER0, (LETIMER_IEN_COMP1));

	CORE_ATOMIC_IRQ_ENABLE();
}

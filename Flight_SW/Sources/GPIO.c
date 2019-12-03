/*
 * GPIO.c
 *
 *  Created on: Feb 27, 2015
 *      Author: Erik Johnson
 */

#include "derivative.h"
#include "GPIO.h"
#include "stdlib.h" // include the code for dynamic allocations

/** BEGIN LED Funcitonality **/

void Init_LEDs()
{
	// Enable clocks to ports that will be used
	SIM_BASE_PTR->SCGC5 |= (SIM_SCGC5_PORTE_MASK | SIM_SCGC5_PORTA_MASK);
	
	// Set the individual pin's MUX to configure them as GPIO
	PORTE_PCR24 |= GPIO_MUX;
	PORTE_PCR25 |= GPIO_MUX;
	PORTA_PCR1 |= GPIO_MUX;
	PORTA_PCR2 |= GPIO_MUX;
	PORTA_PCR4 |= GPIO_MUX;
	PORTA_PCR5 |= GPIO_MUX;
	PORTA_PCR12 |= GPIO_MUX;
	PORTA_PCR13 |= GPIO_MUX;
	
	// Now configure the pins as outputs
	GPIOE_PDDR |= (LED1_E | LED2_E);
	GPIOA_PDDR |= (LED3_A | LED4_A | LED5_A | LED6_A | LED7_A | LED8_A);
	
	// Toggle all pins off initially (LEDs off)
	GPIOE_PCOR |= (LED1_E | LED2_E);
	GPIOA_PCOR |= (LED3_A | LED4_A | LED5_A | LED6_A | LED7_A | LED8_A);
}

void LightAll()
{
	GPIOE_PSOR |= (LED1_E | LED2_E);
	GPIOA_PSOR |= (LED3_A | LED4_A | LED5_A | LED6_A | LED7_A | LED8_A);
}

void DarkenAll()
{
	GPIOE_PCOR |= (LED1_E | LED2_E);
	GPIOA_PCOR |= (LED3_A | LED4_A | LED5_A | LED6_A | LED7_A | LED8_A);
}

void Light(int led)
{
	if (led > 8 || led < 1)
		return;
	
	switch (led)
	{
	case 1:
		GPIOE_PSOR |= LED1_E;
		break;
	case 2:
		GPIOE_PSOR |= LED2_E;
		break;
	case 3:
		GPIOA_PSOR |= LED3_A;
		break;
	case 4:
		GPIOA_PSOR |= LED4_A;
		break;
	case 5:
		GPIOA_PSOR |= LED5_A;
		break;
	case 6:
		GPIOA_PSOR |= LED6_A;
		break;
	case 7:
		GPIOA_PSOR |= LED7_A;
		break;
	case 8:
		GPIOA_PSOR |= LED8_A;
		break;
	default:
		return;
	}
}

void Darken(int led)
{
	if (led > 8 || led < 1)
		return;
	
	switch (led)
	{
	case 1:
		GPIOE_PCOR |= LED1_E;
		break;
	case 2:
		GPIOE_PCOR |= LED2_E;
		break;
	case 3:
		GPIOA_PCOR |= LED3_A;
		break;
	case 4:
		GPIOA_PCOR |= LED4_A;
		break;
	case 5:
		GPIOA_PCOR |= LED5_A;
		break;
	case 6:
		GPIOA_PCOR |= LED6_A;
		break;
	case 7:
		GPIOA_PCOR |= LED7_A;
		break;
	case 8:
		GPIOA_PCOR |= LED8_A;
		break;
	default:
		return;
	}
}

/** END LED Functionality **/

/** BEGIN Jumper Reading Functionality **/

void Init_Jumpers()
{
	// Enable clocks to ports that will be used
	SIM_BASE_PTR->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	
	// Set the individual pin's MUX to configure them as GPIO
	PORTD_PCR2 |= GPIO_MUX;
	PORTD_PCR3 |= GPIO_MUX;
	PORTD_PCR4 |= GPIO_MUX;
	
	// Pins are inputs by default
}

// Returns 1 if jumper in place and 0 if no jumper present
int ReadJumper(int jumper)
{
	if (jumper > 3 || jumper < 1)
		return -1;
	
	switch (jumper)
	{
	case 1:
		return (JP4 & GPIOD_PDIR);
	case 2:
		return (JP5 & GPIOD_PDIR);
	case 3:
		return (JP6 & GPIOD_PDIR);
	default:
		return -1;
	}
}

// [JP4, JP5, JP6] with meanings as above
/*int* ReadAllJumpers()
{
	int* rtn = malloc(3*sizeof(int));
	rtn[0] = ReadJumper(1);
	rtn[1] = ReadJumper(2);
	rtn[2] = ReadJumper(3);
	return rtn;
}*/

/** END Jumper Reading Functionality **/

/** BEGIN Other GPIO Functionality **/

void Init_GPIO()
{
	// Enable clocks to ports that will be used
	SIM_BASE_PTR->SCGC5 |= SIM_SCGC5_PORTC_MASK;
	
	// Set the individual pin's MUX to configure them as GPIO
	PORTC_PCR0 |= GPIO_MUX; // Buzzer pin
	PORTC_PCR10	|= GPIO_MUX; // Release mechanism pin 1
	PORTC_PCR11 |= GPIO_MUX; // Release mechanism pin 2
	
	// Now configure the pins as outputs
	GPIOC_PDDR |= (BUZZER_PIN_MASK | RM1_PIN_MASK | RM2_PIN_MASK);
	
	// Toggle all pins off initially
	GPIOC_PCOR |= (BUZZER_PIN_MASK | RM1_PIN_MASK | RM2_PIN_MASK);
}

void ActivateLocationAssistanceDevice()
{
	GPIOC_PSOR |= BUZZER_PIN_MASK;
}

void ReleaseFromContainer()
{
	GPIOC_PSOR |= RM1_PIN_MASK;
}

void ReleaseBlades()
{
	GPIOC_PSOR |= RM2_PIN_MASK;
}

void StopReleaseFromContainer()
{
	GPIOC_PCOR |= RM1_PIN_MASK;
}

void StopReleaseBlades()
{
	GPIOC_PCOR |= RM1_PIN_MASK;
	GPIOC_PCOR |= RM2_PIN_MASK;
}

/** END Other GPIO Functionality **/

/*
 * PWM.c
 *
 *  Created on: Feb 27, 2015
 *      Author: Erik Johnson
 */

#include "derivative.h"
#include "MotorControl.h"
#include "math.h"

/* Datasheet quotes begin here: */
// The frequency of the input PWM signal can be between 20kHz and 100kHz.
// We want operating modes d,g,h from table 4 on page 10
// A diagram of this operating mode is found on page 11

void Init_MotorControl()
{
	// Port the clock to the TPM to be used for PWM
	SIM_BASE_PTR->SCGC6 |= SIM_SCGC6_TPM0_MASK;
	SIM_BASE_PTR->SOPT2 |= SIM_SOPT2_TPMSRC(0b11); // MCGIRCLK -> 2 MHz
	
	// Set up the PWM module to be in a default state
	TPM0_SC |= TPM_SC_CMOD(0b01); // Sets TPM0 counter to increment with TPM0 counter clock
	TPM0_SC |= TPM_SC_PS(PWM_PRESCALER); // Sets the TPM0 prescaler
	uint16_t modvalue = (TPM0_CLK_FREQ/(pow(PWM_PRESCALER,2)))/PWM_FREQ; // note: don't change
	TPM0_MOD = modvalue;
	
	// Port the clock to the pins to be configured for PWM
	SIM_BASE_PTR->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	
	// Set the motor control pins to be in high state (mode d -> stop)
	// Set both PTD0 and PTD1 to be GPIO mode
	PORTD_PCR0 |= PORT_PCR_MUX(1);
	PORTD_PCR1 |= PORT_PCR_MUX(1);
	// Set both PTD0 and PTD1 to be high (mode d)
	GPIOD_PSOR |= (MOTOR_FWD_MASK | MOTOR_BWD_MASK);
}

void Motor_FWD (uint8_t drivepercent)
{
	if (~(PORTD_PCR0 | PORT_PCR_MUX(4)))
	{
		// Clear pin modes
		PORTD_PCR0 &= ~(PORT_PCR_MUX(0b111)); PORTD_PCR1 &= ~(PORT_PCR_MUX(0b111));
		// Set PTD0 to be PWM mode
		PORTD_PCR0 |= PORT_PCR_MUX(4); // ALT4 for PTD0 is TPM0_CH0 (this is motor FWD pin)
		// Set PTD1 to be GPIO mode
		PORTD_PCR1 |= PORT_PCR_MUX(1); // *** check default state
		// Set PTD1 to be output
		GPIOD_PDDR |= MOTOR_BWD_MASK;
		// Set PTD1 to be high
		GPIOD_PSOR |= MOTOR_BWD_MASK;
	}
	// Low-true pulses (set Output on match, clear Output on reload)
	TPM0_C0SC |= (TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK);
	// Correct if drive percent is over 100
	if (drivepercent > 100)
		drivepercent = 100;
	// Set counter match value
	TPM0_C0V = (TPM0_MOD&0xFFFF)*((float)drivepercent/100.0);
}

void Motor_BWD (uint8_t drivepercent)
{
	if (~(PORTD_PCR1 | PORT_PCR_MUX(4)))
	{
		// Clear pin modes
		PORTD_PCR0 &= ~(PORT_PCR_MUX(0b111)); PORTD_PCR1 &= ~(PORT_PCR_MUX(0b111));
		// Set PTD1 to be PWM mode
		PORTD_PCR1 |= PORT_PCR_MUX(4); // ALT4 for PTD1 is TPM0_CH1 (this is motor BWD pin)
		// Set PTD0 to be GPIO mode
		PORTD_PCR0 |= PORT_PCR_MUX(1); // *** check default state
		// Set PTD0 to be output
		GPIOD_PDDR |= MOTOR_FWD_MASK;
		// Set PTD0 to be high
		GPIOD_PSOR |= MOTOR_FWD_MASK;
	}
	// Low-true pulses (set Output on match, clear Output on reload)
	TPM0_C1SC |= (TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK);
	// Correct if drive percent is over 100
	if (drivepercent > 100)
		drivepercent = 100;
	// Set counter match value
	TPM0_C1V = (TPM0_MOD&0xFFFF)*((float)drivepercent/100.0);
}

void Motor_Stop()
{
	// Set the motor control pins to be in high state (mode d -> stop)
	// Set both PTD0 and PTD1 to be GPIO mode
	PORTD_PCR0 |= PORT_PCR_MUX(1);
	PORTD_PCR1 |= PORT_PCR_MUX(1);
	// Set both PTD0 and PTD1 to be high Z (motor a)
	GPIOD_PDDR &= ~(MOTOR_FWD_MASK | MOTOR_BWD_MASK);
}

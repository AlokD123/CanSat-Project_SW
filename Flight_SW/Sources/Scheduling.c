/*
 * Scheduling.c
 *
 *  Created on: Dec 14, 2014
 *      Author: Erik Johnson
 *      
 * This file holds the code for setting up the timers to schedule the time critical events.
 */

#include "derivative.h" // include peripheral declarations
#include "Scheduling.h" // include the header file

/*void Init_TPM0(int ms_timeout)
{
	// Port the clock to the TPM in question (TPM0)
	SIM_BASE_PTR->SCGC6 |= SIM_SCGC6_TPM0_MASK;
	
	// Set the timer to increment with the clock, with a prescaler of 128
	TPM0_BASE_PTR->SC |= TPM_SC_CMOD(1) | TPM_SC_PS(0b111);
	
	// Set the match value for the timer
	TPM0_BASE_PTR->MOD = (int)((TPM_CLK_SCALE*(float)ms_timeout/1000.0)*((float)TPM_CLK_FREQ/(float)128));
	
	// Enable interrupts for the TPM in question (TPM0)
	TPM0_BASE_PTR->SC |= TPM_SC_TOIE_MASK;
	NVIC_BASE_PTR->ISER = 1<<17;
}*/

void Init_TPM1(int ms_timeout) // Obsolete
{
	// Port the clock to the TPM in question (TPM1)
	SIM_BASE_PTR->SCGC6 |= SIM_SCGC6_TPM1_MASK;
	
	// Set the timer to increment with the clock, with a prescaler of 128
	TPM1_BASE_PTR->SC |= TPM_SC_CMOD(1) | TPM_SC_PS(0b111);
	
	// Set the match value for the timer
	TPM1_BASE_PTR->MOD = (int)(((float)ms_timeout/1000.0)*(((float)TPM_CLK_KHZ * 1000.0)/(float)128));
	
	// Enable interrupts for the TPM in question (TPM1)
	TPM1_BASE_PTR->SC |= TPM_SC_TOIE_MASK;
	NVIC_BASE_PTR->ISER = 1<<18;
}

/*void Init_Scheduling(int ms_transmit_T, int ms_save_T) // Obsolete
{
	Init_TPM0(ms_transmit_T);
	Init_TPM1(ms_save_T);
}*/

void Init_Servicing(int ms_servicing_T)
{
	Init_TPM1(ms_servicing_T);
}

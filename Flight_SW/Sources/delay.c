/*
 * delay.c
 *
 *  Created on: Feb 28, 2015
 *      Author: Alok Deshpande
 */

#include "derivative.h"
#include "delay.h"

// delay(ms) -- Spin wait delay (in ms)
//              Note:  uses low power timer (LPTMR)
void delay(unsigned int length_ms)
{
    SIM_SCGC5 |= SIM_SCGC5_LPTMR_MASK;  // Make sure clock is enabled
    LPTMR0_CSR = 0;                     // Reset LPTMR settings (also resets TCF!)      
    LPTMR0_CMR = length_ms;             // Set compare value (in ms)

    // Use 1kHz LPO with no prescaler
    LPTMR0_PSR = LPTMR_PSR_PCS(1) | LPTMR_PSR_PBYP_MASK;
    
    // Start the timer and wait for it to reach the compare value
    LPTMR0_CSR = LPTMR_CSR_TEN_MASK;
    while (!(LPTMR0_CSR & LPTMR_CSR_TCF_MASK))
        ;
    
    LPTMR0_CSR = 0; // Turn off timer (also resets TCF!)
}

void StartTimer (unsigned int length_ms)
{
	SIM_SCGC5 |= SIM_SCGC5_LPTMR_MASK;  // Make sure clock is enabled
	LPTMR0_CSR = 0;                     // Reset LPTMR settings (also resets TCF!)      
	LPTMR0_CMR = length_ms;             // Set compare value (in ms)

	// Use 1kHz LPO with no prescaler
	LPTMR0_PSR = LPTMR_PSR_PCS(1) | LPTMR_PSR_PBYP_MASK;
	
	// Start the timer
	LPTMR0_CSR = LPTMR_CSR_TEN_MASK;
}

void WaitForTimer()
{
	// Wait for timer to timeout, if it hasn't already
	while (!(LPTMR0_CSR & LPTMR_CSR_TCF_MASK))
		;
	
	// Turn the timer off (also resets TCF!)
	LPTMR0_CSR = 0;
}

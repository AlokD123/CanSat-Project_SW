/*
 * UART.c
 *
 *  Created on: Mar 8, 2015
 *      Author: Erik Johnson
 *      
 * Source file for UART communication driver.
 */

#include "derivative.h"
#include "UART.h"

/*
 * PTE20 is UART0 TX
 * PTE21 is UART0 RX
 * PTE22 is UART2 TX
 * PTE23 is UART2 RX
 */

void Xbee_Init(int baudrate) // Xbee is on UART0
{
	/* Enable the UART pins and set them to their UART function */ // In this case UART0 (PTE 20/21)
	SIM_BASE_PTR->SCGC5 |= SIM_SCGC5_PORTE_MASK; // Clock to port E
	PORTE_PCR20 |= UART_MUX; // Set to UART alternate function 4
	PORTE_PCR21 |= UART_MUX; // Set to UART alternate function 4
	
	/* Enable the clock to the selected UART */ // In this case UART0
	SIM_BASE_PTR->SCGC4 |= SIM_SCGC4_UART0_MASK;
	SIM_BASE_PTR->SOPT2 |= SIM_SOPT2_UART0SRC(0b11); // MCGIRCLK is ported to UART0
	
	/* Make sure TX/RX is disabled for changing settings */
	UART0_C2 |= 0; // Clear control register 2 disables TX/RX
	
	/* Configure UART */ // In this case 8-bit, no parity (stop bits set in Baud Rate Register High)
	UART0_C1 |= 0;
	
	/* Calculate the baud rate setting */
	uint16_t sbr = (uint16_t)((UART0_CLK_KHZ*1000)/(baudrate * 16));
	
	/* Save the current value of the UART BDH register, except for SBR field */
	uint16_t bdh = UART0_BASE_PTR->BDH & ~(UART_BDH_SBR(0x1F));
	
	/* Set the baud rate */
	UART0_BASE_PTR->BDH = bdh | UART_BDH_SBR((sbr & 0x1F00) >> 8);
	UART0_BASE_PTR->BDL = (uint8_t)(sbr & UART_BDL_SBR_MASK);
	
	/* Enable TX/RX */
	UART0_C2 |= (UART0_C2_RE_MASK | UART0_C2_TE_MASK);
}

void Xbee_WriteChar(char ch) // Xbee is on UART0
{
	/* Wait until space is available in the FIFO */
	while(!(UART0_S1 & UART0_S1_TDRE_MASK));
	
	/* Send the character */
	UART0_D = (uint8_t)ch;
 }

void Xbee_WriteString(char str[])
{
	int i;
	for (i = 0; str[i] != '\0'; i++)
	{
		Xbee_WriteChar(str[i]);
	}
}

void Xbee_WriteByte(uint8_t byte)
{
	/* Wait until space is available in the FIFO */
	while(!(UART0_S1 & UART0_S1_TDRE_MASK));
	
	/* Send the character */
	UART0_D = byte;
}

void Xbee_WriteArrayBytes(uint8_t bytes[], int n)
{
	int i;
	for (i = 0; i < n; i++)
	{
		Xbee_WriteByte(bytes[i]);
	}
}

void Xbee_Write16(uint16_t val)
{
	int i;
	for (i = 0; i < 2; i++)
	{
		// LSB first (i.e. little endian)
		Xbee_WriteByte((uint8_t)(val<<(i*8) & 0xFF));
	}
}

void Xbee_WriteArray16(uint16_t vals[], int n)
{
	int i;
	for (i = 0; i < n; i++)
	{
		Xbee_Write16(vals[i]);
	}
}

void Xbee_Write32(uint32_t val)
{
	int i;
	for (i = 0; i < 4; i++)
	{
		// LSB first (i.e. little endian)
		Xbee_WriteByte((uint8_t)(val<<(i*8) & 0xFF));
	}
}

void Xbee_WriteArray32(uint32_t vals[], int n)
{
	int i;
	for (i = 0; i < n; i++)
	{
		Xbee_Write32(vals[i]);
	}
}

void PC_Init(int baudrate)
{
	/* Enable the UART pins and set them to their UART function */ // In this case UART0 (PTE 20/21)
	SIM_BASE_PTR->SCGC5 |= SIM_SCGC5_PORTE_MASK; // Clock to port E
	PORTE_PCR22 |= UART_MUX; // Set to UART alternate function 4
	PORTE_PCR23 |= UART_MUX; // Set to UART alternate function 4
	
	/* Enable the clock to the selected UART */ // In this case UART2
	SIM_BASE_PTR->SCGC4 |= SIM_SCGC4_UART2_MASK;
	
	/* Make sure TX/RX is disabled for changing settings */
	UART2_C2 |= 0; // Clear control register 2 disables TX/RX
	
	/* Configure UART */ // In this case 8-bit, no parity (stop bits set in Baud Rate Register High)
	UART2_C1 |= 0;
	
	/* Calculate the baud rate setting */
	uint16_t sbr = (uint16_t)((UART2_CLK_KHZ*1000)/(baudrate * 16));
	
	/* Save the current value of the UART BDH register, except for SBR field */
	uint16_t bdh = UART2_BASE_PTR->BDH & ~(UART_BDH_SBR(0x1F));
	
	/* Set the baud rate */
	UART2_BASE_PTR->BDH = bdh | UART_BDH_SBR((sbr & 0x1F00) >> 8);
	UART2_BASE_PTR->BDL = (uint8_t)(sbr & UART_BDL_SBR_MASK);
	
	/* Enable RX interrupts */
	UART2_C2 |= UART_C2_RIE_MASK;
	NVIC_BASE_PTR->ISER = 1<<14;
	
	/* Enable TX/RX */
	UART2_C2 |= (UART_C2_RE_MASK | UART_C2_TE_MASK);
}

void PC_WriteChar(char ch)
{
	/* Wait until space is available in the FIFO */
	while(!(UART2_S1 & UART_S1_TDRE_MASK));
	
	/* Send the character */
	UART2_D = (uint8_t)ch;
}

void PC_WriteString(char str[])
{
	int i;
	for (i = 0; str[i] != '\0'; i++)
	{
		PC_WriteChar(str[i]);
	}
}

void PC_WriteByte(uint8_t byte)
{
	/* Wait until space is available in the FIFO */
	while(!(UART2_S1 & UART_S1_TDRE_MASK));
	
	/* Send the character */
	UART2_D = byte;
}

void PC_WriteArrayBytes(uint8_t bytes[], int n)
{
	int i;
	for (i = 0; i < n; i++)
	{
		PC_WriteByte(bytes[i]);
	}
}

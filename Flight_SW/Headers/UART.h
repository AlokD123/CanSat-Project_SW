/*
 * UART.h
 *
 *  Created on: Mar 8, 2015
 *      Author: Erik Johnson
 *      
 * Header file for UART communication driver.
 */

#ifndef UART_H_
#define UART_H_

#define UART0_CLK_KHZ 4000 // MCGIRCLK in kHz (as setup in main->BasicInit)
#define UART2_CLK_KHZ 4000 // Core clock in kHz (as setup in main->BasicInit)
#define UART_MUX 0b100<<8

// Xbee is on UART0

void Xbee_Init(int baudrate);
void Xbee_WriteChar(char ch);
void Xbee_WriteString(char str[]);
void Xbee_WriteByte(uint8_t byte);
void Xbee_WriteArrayBytes(uint8_t bytes[], int n);
void Xbee_Write16(uint16_t val);
void Xbee_WriteArray16(uint16_t vals[], int n);
void Xbee_Write32(uint32_t val);
void Xbee_WriteArray32(uint32_t vals[], int n);
void PC_Init(int baudrate);
void PC_WriteChar(char ch);
void PC_WriteString(char str[]);
void PC_WriteByte(uint8_t byte);
void PC_WriteArrayBytes(uint8_t bytes[], int n);

#endif /* UART_H_ */

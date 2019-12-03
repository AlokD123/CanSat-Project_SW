/*
 * AT24CM01.h
 *
 *  Created on: Mar 18, 2015
 *      Author: Alok Deshpande
 */


#ifndef AT24CM01_H_
#define AT24CM01_H_

#include "stdint.h"
#include "MPU6500.h"

/*	
 * Creating the CurrentAddress for the EEPROM, use this
 */
/*
	uint32_t CurrentAddress=0; // The value is zero at the start, however, at a reset, make sure you make this the CurrentAddress value read from the RTC's EEPROM
	uint32_t *CurrentAddressPointer;
	CurrentAddressPointer = &CurrentAddress;
*/

/*	
 * Creating the ReadAddress for the EEPROM, use this
 */
/*
	uint32_t ReadAddress=0;
	uint32_t *ReadAddressPointer;
	ReadAddressPointer = &ReadAddress;
*/

void AT_WriteByte(uint32_t CurrentAddress, uint8_t Data);
void AT_WriteMultiBytes(uint32_t CurrentAddress, uint8_t Data[], int sizeofdata);
void AT_ReadMultiBytes(uint32_t CurrentAddress, uint8_t *Data, int Bytes);
void AT_Writes(uint32_t *CurrentAddress, uint8_t Data[], int sizeofdata);
uint8_t AT_Reads(uint32_t *ReadAddress, uint32_t *LastWrite, uint8_t *Data);
void AT_Clear();

#endif /* AT24CM01_H_ */

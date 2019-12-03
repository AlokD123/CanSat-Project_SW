/*
 * AT24CM01.C
 *
 *  Created on: Mar 18, 2015
 *      Author: Alok Deshpande
 */

#include "derivative.h"
#include "I2C.h"
#include "stdint.h"
#include "math.h"
#include "AT24CM01.h"
#include "delay.h"
#include "MPU6500.h"


#define EEPROMAddress 0x28 // CHECK SCHEMATIC
#define MaxAddress 255

void AT_WriteByte(uint32_t CurrentAddress, uint8_t Data){
	uint8_t DeviceAddress,AddressMSB,AddressLSB;
	AddressLSB = (uint8_t)CurrentAddress;
	AddressMSB = (uint8_t)(CurrentAddress>>8);
	DeviceAddress = ((EEPROMAddress<<2)|(uint8_t)(CurrentAddress>>15))&(0xFE);
	
	I2C0_Start();	          
	I2C0_D = DeviceAddress;									
	if(I2C0_Wait() == 0x00){
		I2C0_Stop();
		return;
	}

	I2C0_D = AddressMSB;	
	if(I2C0_Wait() == 0x00){
		I2C0_Stop();
		return;
	}

	I2C0_D = AddressLSB;	
	if(I2C0_Wait() == 0x00){
		I2C0_Stop();
		return;
	}

	I2C0_D = Data;	
	if(I2C0_Wait() == 0x00){
		I2C0_Stop();
		return;
	}
	
	CurrentAddress++;

	I2C0_Stop();

    Pause(50);
}

void AT_WriteMultiBytes(uint32_t CurrentAddress, uint8_t Data[], int sizeofdata){
	uint8_t DeviceAddress,AddressMSB,AddressLSB;
	AddressLSB = (uint8_t)CurrentAddress;
	AddressMSB = (uint8_t)(CurrentAddress>>8);
	DeviceAddress = ((EEPROMAddress<<2)|(uint8_t)(CurrentAddress>>15))&(0xFE);
	
	I2C0_Start();	          
	I2C0_D = DeviceAddress;									
	if(I2C0_Wait() == 0x00){
		I2C0_Stop();
		return;
	}

	I2C0_D = AddressMSB;	
	if(I2C0_Wait() == 0x00){
		I2C0_Stop();
		return;
	}

	I2C0_D = AddressLSB;	
	if(I2C0_Wait() == 0x00){
		I2C0_Stop();
		return;
	}
	
	int i=0;
	for(;i<sizeofdata;i++){
		I2C0_D = Data[i];	
		if(I2C0_Wait() == 0x00){
			I2C0_Stop();
			return;
		}
	}
	I2C0_Stop();

    Pause(50);
}

void AT_ReadMultiBytes(uint32_t CurrentAddress, uint8_t *Data, int Bytes){
	uint8_t DeviceAddress,AddressMSB,AddressLSB;
	AddressLSB = (uint8_t)CurrentAddress;
	AddressMSB = (uint8_t)(CurrentAddress>>8);
	DeviceAddress = ((EEPROMAddress<<2)|(uint8_t)(CurrentAddress>>15))&(0xFE);
	
	I2C0_Start();	          
	I2C0_D = DeviceAddress;									
	if(I2C0_Wait() == 0x00){
		I2C0_Stop();
		return;
	}

	I2C0_D = AddressMSB;	
	if(I2C0_Wait() == 0x00){
		I2C0_Stop();
		return;
	}

	I2C0_D = AddressLSB;	
	if(I2C0_Wait() == 0x00){
		I2C0_Stop();
		return;
	}
	
	I2C0_RepeatedStart();
		
	I2C0_D = ((DeviceAddress) | 0x01);							
	if(I2C0_Wait() == 0x00){
		I2C0_Stop();
		return;
	}	

	I2C0_EnterRxMode();
	I2C0_EnableAck();
	Data[0] = I2C0_D;
	if(I2C0_Wait() == 0x00){
		I2C0_Stop();
		return;
	}
	
	int i = 0;
	for(;i<Bytes;i++){
		Data[i] = I2C0_D;
		if(I2C0_Wait() == 0x00){
			I2C0_Stop();
			return;
		}
		if(i+2==Bytes){
			I2C0_DisableAck();	
		}
	}
	// Stopping I2C transmit and making the device a slave
	I2C0_Stop();
	
	Pause(50);
	return;
}

/*	This function writes the packets of data to the EEPROM
 * 	Data is an array of size (sizeofdata). The array contains the data in the order agreed on. sizeofdata contains the size of the array
 * 	CurrentAddress is a pointer to the current address at which writes can be done
 * 	This function takes care of the pages in the eeprom
 */
void AT_Writes(uint32_t *CurrentAddress, uint8_t Data[], int sizeofdata){
	// taking the LSB of the address
	uint8_t AddressLSB = (uint8_t)*CurrentAddress;
	// Checking if the space left in the current page is enough to fill in the entire array of data
	if((MaxAddress-AddressLSB+1)>=sizeofdata){
		// the space in the page is enough, the data is written and the currentaddress is incremented by the sizeofdata
		AT_WriteMultiBytes(*CurrentAddress, Data, sizeofdata);
		*CurrentAddress = *CurrentAddress + sizeofdata;
	}else{
		// the space is not enough, therefore the data is written at the start of the next page, and the current address is incremented from the start of the next page
		*CurrentAddress += (MaxAddress-AddressLSB)+1;
		AT_WriteMultiBytes(*CurrentAddress, Data, sizeofdata);
		*CurrentAddress = *CurrentAddress + sizeofdata;
	}
}

/* This function returns the data in the pages of the eeprom. Data is an array of 256 bytes (MUST BE)
 * Every read will return the data from the eeprom and the read address for the next read.
 * Parameters: ReadAddress; contains the address from which the read should start
 * 				LastWrite; contains the address at which the last write address + saving size. The address at which the last written data was put +1.
 * 				Data; a pointer to the 256 bytes array that will contain the returned data
 * Returns: true: reading was successful, the page has valid data in it
 * 			false: reading was not successful, the page is completely empty. The last page contained the last written data.
 */
uint8_t AT_Reads(uint32_t *ReadAddress, uint32_t *LastWrite, uint8_t *Data){
	// if the ReadAddress is larger than the LastWrite address then the page is empty (therefore return 0x00)
	if(*ReadAddress>=*LastWrite){
		return 0x00;
	}else{
		// Page is not empty, read 256 bytes and increment the readaddress by 256 then return 0xFF to let the main function know the read was successful
		AT_ReadMultiBytes(*ReadAddress, Data, MaxAddress+1);
		*ReadAddress = *ReadAddress+MaxAddress+1;
		return 0xFF;
	}
}

/*	This function clears the EEPROM by writing 0 to all memory locations
 */
void AT_Clear(){
	uint32_t CurrentAddress = 0;
	uint32_t *CurrentAddressPointer;
	CurrentAddressPointer = &CurrentAddress;
	uint8_t data[256];
	int n=0;
	for(;n<256;n++){
		data[n]=0;
	}
	
	n=0;
	for(;n<512;n++){
		AT_Writes(CurrentAddressPointer, data, (int)256);
		delay(1);
	}
}


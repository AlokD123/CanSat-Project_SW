/*
 * DS1338.c
 *
 *  Created on: Feb 13, 2015
 *      Author: Alok Deshpande
 */

#include "derivative.h"
#include "I2C.h"
#include "stdint.h"
#include "math.h"
#include "DS1338.h"

#define DSAddress 0x68
#define SecondsAddress 0x00
#define MinutesAddress 0x01
#define HoursAddress 0x02
#define DaysAddress 0x03
#define DatesAddress 0x04
#define MonthsAddress 0x05
#define YearsAddress 0x06
#define ControlAddress 0x07

#define HourMode24 0xBF

#define	CurrentState 0x08
#define EEPROMCurrentAddress 0x09
#define InitialPressure 0x0C
#define	MagnoXCalMin 0x10
#define MagnoXCalMax 0x12
#define	MagnoYCalMin 0x14
#define MagnoYCalMax 0x16

void RTC_SelectMode(){
	uint8_t temp = I2C0_ReadRegister(DSAddress,HoursAddress);
	temp &= HourMode24;
	I2C0_WriteRegister(DSAddress,HoursAddress,temp);
}

void RTC_SetTime(uint8_t hour, uint8_t minute, uint8_t second){
	
	
	uint8_t temp = hour;
	temp &= HourMode24;
	I2C0_WriteRegister(DSAddress,HoursAddress,temp);
	
	I2C0_WriteRegister(DSAddress,MinutesAddress,minute);
	
	temp = second & 0x7F;
	I2C0_WriteRegister(DSAddress,SecondsAddress,temp);
}

void RTC_SetControl(){
	I2C0_WriteRegister(DSAddress,ControlAddress,0x00);
}

RTC_Data RTC_GetData(){
	RTC_Data results;
	uint8_t temp[3];
	I2C0_MultiByteRead(DSAddress,SecondsAddress,temp, 3);
	
	temp[0] = BCD_Dec(temp[0],0);
	results.seconds = temp[0];

	temp[1] = BCD_Dec(temp[1],0);
	results.minutes = temp[1];
	
	temp[2] = BCD_Dec(temp[2],1);
	results.hours = temp[2];
	return results;
}

uint8_t BCD_Dec(uint8_t data, uint8_t type){
	uint8_t temp1 = data;
	uint8_t temp2 = data;
	uint8_t result;
	temp1 &= 0x0F;
	if(type == 0){
		temp2 &= 0x70;
	}else{
		temp2 &= 0x30;
	}
	temp2 = temp2>>4;
	result = (temp2*10)+temp1;
	return result;
}

void RTC_SaveEEPROMAddress(uint32_t CurrentAddress){
	uint8_t Data[3];
	Data[0] = (uint8_t)CurrentAddress; // LSB
	Data[1] = (uint8_t)(CurrentAddress>>8); // MSB
	Data[2] = (uint8_t)(CurrentAddress>>16); //XMSB
	
	I2C0_WriteMultiByte(DSAddress,EEPROMCurrentAddress, Data,3);
}

uint32_t RTC_GetEEPROMAddress(){
	uint8_t Data[3];
	
	uint32_t CurrentAddress=0;
	
	I2C0_MultiByteRead(DSAddress,EEPROMCurrentAddress,Data, 3);
	
	CurrentAddress |= Data[2];
	CurrentAddress <<= 8;
	CurrentAddress |= Data[1];
	CurrentAddress <<= 8;
	CurrentAddress |= Data[0];

	return CurrentAddress;
}

void RTC_SaveCurrentState(uint8_t State){
	I2C0_WriteRegister(DSAddress,CurrentState,State);
}

uint8_t RTC_GetCurrentState(){
	return (I2C0_ReadRegister(DSAddress,CurrentState));
}

void RTC_SaveInitialPressure(int32_t pressure){
	uint32_t initPressure = (uint32_t)pressure;
	
	uint8_t Data[4];
	Data[0] = (uint8_t)initPressure; // XLSB
	Data[1] = (uint8_t)(initPressure>>8); // LSB
	Data[2] = (uint8_t)(initPressure>>16); //MSB
	Data[3] = (uint8_t)(initPressure>>24); //XMSB
	
	I2C0_WriteMultiByte(DSAddress,InitialPressure, Data,4);
}

int32_t RTC_GetInitialPressure(){
	uint8_t Data[4];
	
	int32_t initPressure=0;
	
	I2C0_MultiByteRead(DSAddress,InitialPressure,Data, 4);
	
	initPressure |= Data[3];
	initPressure <<= 8;
	initPressure |= Data[2];
	initPressure <<= 8;
	initPressure |= Data[1];
	initPressure <<= 8;
	initPressure |= Data[0];

	return initPressure;
}

void RTC_SaveMagnoXCal_Min(int16_t magnoXCal){
	uint16_t magXCal = (uint16_t)magnoXCal;
	
	uint8_t Data[2];
	Data[0] = (uint8_t)magXCal; // LSB
	Data[1] = (uint8_t)(magXCal>>8); // MSB
	
	I2C0_WriteMultiByte(DSAddress,MagnoXCalMin, Data,2);
}

int16_t RTC_GetMagnoXCal_Min(){
	uint8_t Data[2];
	
	int16_t magXCal=0;
	
	I2C0_MultiByteRead(DSAddress,MagnoXCalMin,Data, 2);
	
	magXCal |= Data[1];
	magXCal <<= 8;
	magXCal |= Data[0];

	return magXCal;
}

void RTC_SaveMagnoYCal_Min(int16_t magnoYCal){
	uint16_t magYCal = (uint16_t)magnoYCal;
	
	uint8_t Data[2];
	Data[0] = (uint8_t)magYCal; // LSB
	Data[1] = (uint8_t)(magYCal>>8); // MSB
	
	I2C0_WriteMultiByte(DSAddress,MagnoYCalMin, Data,2);
}

int16_t RTC_GetMagnoYCal_Min(){
	uint8_t Data[2];
	
	int16_t magYCal=0;
	
	I2C0_MultiByteRead(DSAddress,MagnoYCalMin,Data, 2);
	
	magYCal |= Data[1];
	magYCal <<= 8;
	magYCal |= Data[0];

	return magYCal;
}

void RTC_SaveMagnoXCal_Max(int16_t magnoXCal){
	uint16_t magXCal = (uint16_t)magnoXCal;
	
	uint8_t Data[2];
	Data[0] = (uint8_t)magXCal; // LSB
	Data[1] = (uint8_t)(magXCal>>8); // MSB
	
	I2C0_WriteMultiByte(DSAddress,MagnoXCalMax, Data,2);
}

int16_t RTC_GetMagnoXCal_Max(){
	uint8_t Data[2];
	
	int16_t magXCal=0;
	
	I2C0_MultiByteRead(DSAddress,MagnoXCalMax,Data, 2);
	
	magXCal |= Data[1];
	magXCal <<= 8;
	magXCal |= Data[0];

	return magXCal;
}

void RTC_SaveMagnoYCal_Max(int16_t magnoYCal){
	uint16_t magYCal = (uint16_t)magnoYCal;
	
	uint8_t Data[2];
	Data[0] = (uint8_t)magYCal; // LSB
	Data[1] = (uint8_t)(magYCal>>8); // MSB
	
	I2C0_WriteMultiByte(DSAddress,MagnoYCalMax, Data,2);
}

int16_t RTC_GetMagnoYCal_Max(){
	uint8_t Data[2];
	
	int16_t magYCal=0;
	
	I2C0_MultiByteRead(DSAddress,MagnoYCalMax,Data, 2);
	
	magYCal |= Data[1];
	magYCal <<= 8;
	magYCal |= Data[0];

	return magYCal;
}



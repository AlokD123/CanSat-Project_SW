/*
 * DS1338.h
 *
 *  Created on: Feb 13, 2015
 *      Author: Alok Deshpande
 */

#ifndef DS1338_H_
#define DS1338_H_

typedef struct
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
} RTC_Data;

void RTC_SelectMode();
void RTC_SetTime(uint8_t hour, uint8_t minute, uint8_t second);
void RTC_SetControl();
RTC_Data RTC_GetData();
uint8_t BCD_Dec(uint8_t data, uint8_t type);
void RTC_SaveEEPROMAddress(uint32_t CurrentAddress);
uint32_t RTC_GetEEPROMAddress();
void RTC_SaveCurrentState(uint8_t State);
uint8_t RTC_GetCurrentState();
void RTC_SaveInitialPressure(int32_t pressure);
int32_t RTC_GetInitialPressure();
void RTC_SaveMagnoXCal_Min(int16_t magnoXCal);
int16_t RTC_GetMagnoXCal_Min();
void RTC_SaveMagnoYCal_Min(int16_t magnoYCal);
int16_t RTC_GetMagnoYCal_Min();
void RTC_SaveMagnoXCal_Max(int16_t magnoXCal);
int16_t RTC_GetMagnoXCal_Max();
void RTC_SaveMagnoYCal_Max(int16_t magnoYCal);
int16_t RTC_GetMagnoYCal_Max();

#endif /* DS1338_H_ */

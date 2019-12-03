/*
 * BMP180.h
 * 
 * Author: Maytham Sabbagh
 * Date: Tuesday, November 25th
 * 
 */
 
 
 
#ifndef BMP180_H_
#define BMP180_H_

typedef struct
{
	int32_t temperature;
	int32_t pressure;
} BMP180;

typedef struct
{
	// 0 AC1, 1 AC2, 2 AC3, 3 B1, 4 B2, 5 MB, 6 MC, 7 MD
	int16_t calibration[8];
	// 0 AC4, 1 AC5, 2 AC6
	uint16_t uncalibration[3];
} calibrationData;

/*	The init functions must be called first, these will retrieve the calibration data
 * 	getInitialPressure must be used after to get the ground pressure (MUST BE CALLED AT GROUND) the initial pressure values must be saved in the EEPROM
 * 	
 * 	To get the pressure and the temperature
 * 	use the ask function to call tell the sensor the temperature/pressure is needed (ONE AT A TIME, you can only as for one of them at a time for one sensor)
 * 	delay for 5ms call (delay(5);), then after that you can receive the requested value using the corresponding receive function
 * 	
 * 	The data received from the receive functions is raw data, to do the calculations to change to 0.1 degree C and kPa
 * 	call the CalculateBMP function which returns a BMP structure. Pass the calibration data, the temperature and the pressure for the corresponding sensor
 * 	The function will do the calculations and return the corrected structure. temp and press are the last readings from the temperature and the pressure sensor
 */


// retrieve the calibration data from the sensors 
// BMP1801 = BMP180_1 on board
calibrationData BMP1801_init();
// BMP1802 = BMP180_2 on board
calibrationData BMP1802_init();

// calculates the altitude based on the data and the initial pressure
int32_t getAltitude(BMP180 sensordata, int32_t initialpressure);

// calculated the initial pressure. Must be called at ground and the value must be SAVED
int32_t getInitialPressure1(calibrationData calibdata);
int32_t getInitialPressure2(calibrationData calibdata);

// Ask for temperature
void askTemperature1();
// ask for pressure
void askPressure1();
// Same as above
void askTemperature2();
void askPressure2();

// Wait 4.5ms before receiving the temperature
int16_t receiveTemperature1();
// wait 4.5ms before receiving the pressure
int32_t receivePressure1();
// same as above
int16_t receiveTemperature2();
int32_t receivePressure2();

// Pass the calibration data, the raw temperature, and the raw pressure. It will return the calculated structure.
BMP180 CalculateBMP(calibrationData calibdata, int16_t temp, int32_t press);


/*
calibrationData BMP1801_init_OLD();
calibrationData BMP1802_init_OLD();
int16_t getTemperature1();
int16_t getTemperature2();
int32_t getPressure1();
int32_t getPressure2();
BMP180 ReadBMP1(calibrationData calibdata);
BMP180 ReadBMP2(calibrationData calibdata);

// Wait 4.5ms before receiving the temperature
int16_t receiveTemperature1_OLD();
// wait 4.5ms before receiving the pressure
int32_t receivePressure1_OLD();
int16_t receiveTemperature2_OLD();
int32_t receivePressure2_OLD();

*/	
#endif /* BMP180_H_ */

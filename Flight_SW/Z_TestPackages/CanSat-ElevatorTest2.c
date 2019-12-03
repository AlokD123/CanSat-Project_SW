/*
 * This file will be the main for the RavenKnights CanSat team of 2015.
 * This code is developped for our custom board running an MKL16Z designed by Jonathan.
 * 
 * Contributors: Erik, Maytham, Alok, Amente, Akshay
 * 
 * Current todos:
 * ***** Stabilize immediately after container deployment
 * ***** Consider a method of container deployment that won't kill CanSat
 */


/* BEGIN Include external code */

#include "derivative.h" // include peripheral declarations
#include "Scheduling.h" // include the code for scheduling 
#include "I2C.h" // include the driver for I2C communication
#include "BMP180.h" // include the driver for BMP180 communication
#include "MPU6500.h" // include the driver for MPU6500 communication
#include "AK8963.h" // include the driver for AK8963 communication
#include "GPIO.h" // include the code for handling GPIO
#include "MotorControl.h" // include the code for MotorControl
#include "PID.h" // include the code for PID control
#include "delay.h" // include the delay code
#include "UART.h" // include the driver for UART communication
#include "AT24CM01.h" // include the code for saving to EEPROM
#include "DS1338.h" // include the code for reading from RTC
#include "stdio.h" // include the code for string manipulations
#include "stdlib.h" // include the code for dynamic allocations
#include "ADC.h" // include the code for the voltage reading

/* END Include external code */

#define P_SORT(a,b) { if ((a)>(b)) P_SWAP((a),(b)); }
#define P_SWAP(a,b) { int16_t temp=(a);(a)=(b);(b)=temp; }


/* BEGIN Pre-processor directives for configuration */

//#define DEBUG
//#define JUMPER_TO_SVD
//#define RELEASE_TESTING

#define SERVICE_T 100 // Period for servicing interrupt in ms

// Temporary configuration defines
#define TWO_BMPS
#define EEPROM_PRESENT
#define RTC_PRESENT

// Memory length def'n
#define ALTITUDE_MEMORY_LEN 15 // Number of altitudes to save in RAM

// State transition def'ns
#define ALTITUDE_MEDIAN_LEN 5 // The number of altitude samples to find the median of for state transition decisions
#define LNC_EXIT_ALTITUDE 10 // mission: 50
#define ASC_EXIT_BELOW_APOGEE 5 // mission: 20
#define PDS_EXIT_ALTITUDE 30 // mission: 500
// When we look back SVD_EXIT_SAMPLES_CONSTANT elements back in the historic altitude data and we find the difference
// between that reading and the current altitude is less than SVD_EXIT_DEADBAND we have landed.
#define SVD_EXIT_DEADBAND 2 // mission: 2
#define SVD_EXIT_SAMPLES_CONSTANT 7 // mission: 7

// Baudrate def'ns
#define XBEE_BAUD 9600 // Baudrate for communicating with the xbee
#define PC_BAUD 9600 // Baudrate for communicating with the PC (through the FTDI)

// Burn sequence def'ns
#define BLADE_RELEASE_DELAY 2.9 // The number of seconds to wait after deploying from container before releasing the blades
// Note: BLADE_RELEASE_DELAY also defines the amount of time to keep the container release line high
#define BLADE_RELEASE_TIME 0.1 // The number of seconds to keep the blade release line high

// Stabilization def'ns
// Setting either to 0 entirely disables all associated stabilization code
#define MAG_STABILIZE_SAMPLES 1 // The number of magnetometer samples to average for stabilization
#define GYRO_STABILIZE_SAMPLES 1 // The number of gyroscope samples to average for stabilization

// PID def'ns
/* Tuning factors for magnetometer PID control */
#define KP_MAG 0.5 // The value for the proportional gain for the mag PID (0.5)
#define KI_MAG 0.25 // The value for the integral gain for the mag PID (0.25)
#define KD_MAG 0.25 // The value for the derivative gain for the mag PID (0.25)
#define MAG_FACTOR 1 // The scaling for the mag control (1)
/* Tuning factors for gyroscope PID control */
#define KP_GYRO 0.004 // The value for the proportional gain for the gyro PID (0.004)
#define KI_GYRO 0.015 // The value for the integral gain for the gyro PID (0.015)
#define KD_GYRO 0.003 // The value for the derivative gain for the gyro PID (0.003)
#define GYRO_FACTOR 1 // The scaling for the gyro control (1)

// Testing jumper location (of the three)
#define TEST_JUMPER 1

// PC command keys
#define EEPROM_REQUEST_KEY 0xFE // 0x61 is 'a' in ASCII for testing
#define CALIBRATION_COMMAND 99

// Sensor failure arbitration
#define SENSOR_FAILURE_ARB 5

// Magnetometer calibration def'n
#define MAG_CAL_TIME 30.0

/* END Pre-processor directives for configuration */


/* BEGIN Pre-processor directives */

#define TPM_CLR_MASK 1<<7

/* END Pre-processor directives */


/* BEGIN Type declarations */

// Add the bool type
typedef enum { false, true } bool;

// Define the state tracking variable
typedef enum {
	TST, // TeSTing state
	LNC, // LauNCh state
	ASC, // ASCent state
	PDS, // Pre-Deployment of Science vehicle state
	SVD, // Science Vehicle Descent state
	LND, // LaNDed state
} fsw_states;

typedef struct {
	BMP180 BMPdata_internal;
	MPU6500_Accel ACCdata;
	//MPU6500_Gyro GYRdata; // Not needed for transmitting/saving... sample only when needed (or maybe for debugging)
	AK8963_Data MAGdata;
#ifdef RTC_PRESENT
	RTC_Data RTCdata;
#endif
#ifdef TWO_BMPS
	BMP180 BMPdata_external;
#endif
	uint16_t BattVolts;
} sample_data;

/* END Type declarations */


/* BEGIN Global variable declarations */

/* State variable */
fsw_states FSW_state;

/* Boolean flags */
// Interrupt set flags
bool _transmit; // Signals a transmit should occur
bool _save; // Signals a save should occur
bool _sample; // Signals a sensor sampling should occur
bool _pcrxbyteavailable; // Signals a byte has been received from the PC
// Event set flags
bool _statetransition; // Signals we have just transitioned in states
bool _eraseeeprom; // Signals we have left the testing state and should erase the eeprom
bool _burnsequences; // Signals we are performing the burn sequences
bool _releaseblades; // Signals we should release the CanSat blades
bool _bladesreleased; // Signals we have already released the CanSat blades

/* Sensor failure detection */
uint8_t _externalBMPfaultcnt; // Counts the number of external BMP faults encountered
bool _externalBMPdead; // Signals we have lost the external BMP

/* EEPROM addresses */
uint32_t EEPROM_Address=0;
uint32_t ReadAddress=0;

/* Calibration data */
calibrationData bmp1caldata; // Calibration data for BMP180_1
calibrationData bmp2caldata; // Calibration data for BMP180_2
AK8963_FuseRom magcaldata; // Calibration data for the magnetometer

/* Magnetometer calibration procedure variables */
bool _calinprogress; // Signals we are calibrating the magnetometer
int _caltimer; // Times the calibration procedure
int16_t _maxHX, _minHX; // min and max x component for the mag
int16_t _maxHY, _minHY; // min and max y component for the mag

/* Other globals */
uint8_t _cycleposition; // Keeps track of where we are in the time slices
uint8_t _releasingtimer; // Times the two releasing sequences using the Interrup_Service_TPM1
uint8_t _pcrxbyte; // The byte received from the PC (0 unless _pcrxbyteavailable flag is set)

/* END Global variable declarations */


/* BEGIN Transmit data array */

#define XBEE_ADDRESS  0x0, 0x13, 0xA2, 0x0, 0x40, 0xD6, 0x6E, 0x22
#define TEAM_ID 0x03, 0xE9
#define PACKET_LEN 39 
uint8_t packet[PACKET_LEN] = 
{
 0x7E,
 0x00, 0x23, // Length
 0x10, // Frame Type
 0x00, // Frame ID
 XBEE_ADDRESS,
 0xFF, 0xFE, // 16bit Addr
 0x00, // Broadcast Radius
 0x00, // Option Byte
 TEAM_ID, 
 0x00, 0x00, // Mission Time
 0x00, 0x00, // Altitude as signed 16 bit value
 0x00, 0x00, // Ext Temp as signed 16 bit value
 0x00, 0x00, // Int Temp as signed 16 bit value
 0x00, 0x00, // Voltage
 0x00, 		 // Flight Software State
 0x00, 0x00, // Accel X as signed 16 bit value
 0x00, 0x00, // Accel Y as signed 16 bit value
 0x00, 0x00, // Accel Z	as signed 16 bit value
 0x00, 0x00, // Pkt Cnt
 0x00  // Checksum
};

#define MISSION_TIME_IDX 19
#define ALT_IDX 21
#define EXT_TEMP 23
#define INT_TEMP 25
#define VOLTAGE 27
#define FSW_STATE 29
#define ACCEL_X 30
#define ACCEL_Y 32
#define ACCEL_Z 34
#define PKT_CNT 36
#define CHK_SUM 38

/* END Transmit data array */


/* BEGIN Interrupt handlers */

void Interrupt_Service_TPM1()
{
	TPM1_BASE_PTR->SC |= TPM_CLR_MASK; // Clear interrupt
	
	// Determine what should be done this time slice
	/*
	 * 0 - 100 ms: TRANSMIT
	 * 101 - 200 ms: SAMPLE
	 * 201 - 300 ms: SAVE
	 * 301 - 400 ms: SAMPLE
	 * 401 - 500 ms: SAVE
	 * 501 - 600 ms: SAMPLE
	 * 601 - 700 ms: SAVE
	 * 701 - 800 ms: SAMPLE
	 * 801 - 900 ms: SAVE
	 * 901 - 1000 ms: SAMPLE
	 */
	switch(_cycleposition)
	{
	case 0:
		// Transmit
		if (FSW_state != LND) // Do not transmit in landed state
			{ _transmit = true; }
		else
			{ _transmit = false; }
		break;
	case 1:
		// Sample
		_sample = true;
		break;
	case 2:
		// Save
		if (FSW_state != TST) // Do not save in testing state
			{ _save = true; }
		else
			{ _save = false; }
		break;
	case 3:
		// Sample
		_sample = true;
		break;
	case 4:
		// Save
		if (FSW_state != TST) // Do not save in testing state
			{ _save = true; }
		else
			{ _save = false; }
		break;
	case 5:
		// Sample
		_sample = true;
		break;
	case 6:
		// Save
		if (FSW_state != TST) // Do not save in testing state
			{ _save = true; }
		else
			{ _save = false; }
		break;
	case 7:
		// Sample
		_sample = true;
		break;
	case 8:
		// Save
		if (FSW_state != TST) // Do not save in testing state
			{ _save = true; }
		else
			{ _save = false; }
		break;
	case 9:
		// Sample
		_sample = true;
		break;
	default:
		// Recover from unforeseen issues
		_cycleposition = 0;
		break;
	}
	
	// Circular counter
	if (_cycleposition>=9)
		{ _cycleposition = 0; }
	else
		{ _cycleposition++; }
	
	if (_calinprogress)
	{
		_caltimer++;
		if (_caltimer >= (float)MAG_CAL_TIME*1000.0/(float)SERVICE_T)
		{
			_calinprogress = false;
		}
	}
	
	// Blade release timing
	if (_burnsequences)
	{
		_releasingtimer++;
		if (_releasingtimer >= (float)BLADE_RELEASE_DELAY*1000.0/(float)SERVICE_T && !_bladesreleased)
			{ _releaseblades = true; }
		
		if (_releasingtimer >= (float)BLADE_RELEASE_DELAY*1000.0/(float)SERVICE_T + (float)BLADE_RELEASE_TIME*1000.0/(float)SERVICE_T)
			{ _burnsequences = false; }
	}
}

void UART2_RX_Interrupt()
{
	// Clear interrupt
	// Read status register 1
	uint8_t s1_read = UART2_S1;
	// Check RDRF is set
	if (s1_read | UART_S1_RDRF_MASK)
	{
		// Read the UART data register
		_pcrxbyte = UART2_D;
		if (FSW_state == TST || FSW_state == LND) // If in states that can service PC communication...
			_pcrxbyteavailable = true; // ...flag that data is available.
		else // ...if we should not be receiving PC communication... (could be caused by noise or smthg else)
			_pcrxbyte = 0; // ...clear the data byte and do NOT flag data available.
	}
}

/* END Interrupt handlers */


/* BEGIN Init functions */

void Basic_Init()
{
	/* Prepare clock system -> FBI mode */
	SIM_BASE_PTR->CLKDIV1 &= ~(SIM_CLKDIV1_OUTDIV4(0b111) | SIM_CLKDIV1_OUTDIV1(0b1111)); // Zero all OUTDIV1 bits (divide factor is 1)
	/* Change C1[CLKS] bits in C1 register to 2'b01 so that the internal reference clock is
	selected as the system clock source.*/
	MCG_BASE_PTR->C1 |= MCG_C1_CLKS(0b01); // Set MCGOUTCLK to internal reference
	/* Wait for S[CLKST] bits in the MCG status register to change to 2'b01, indicating
	that the internal reference clock has been appropriately selected.*/
	while((MCG_S_CLKST(0b01) & MCG_BASE_PTR->S) == 0) {}
	/* Write to the C2 register to determine the IRCS output (IRCSCLK) frequency range.*/
	MCG_BASE_PTR->C2 |= MCG_C2_IRCS_MASK; // Select fast internal reference (4 MHz)
	MCG_BASE_PTR->SC &= ~(MCG_SC_FCRDIV(0b111)); // Zero all FCRDIV bits (divide factor is 1)
	MCG_BASE_PTR->C1 |= (MCG_C1_IRCLKEN_MASK | MCG_C1_IREFS_MASK); // Enable MCGIRCLK (used to clock some peripherals) and send slow internal reference to FLL
	MCG_BASE_PTR->C6 &= ~(MCG_C6_PLLS_MASK); // Set PLLS bit to 0 (selects FLL not PLL)
	MCG_BASE_PTR->C2 &= ~(MCG_C2_LP_MASK); // Set LP bit to 0 (FLL/PLL not disabled in bypass mode)
	// This makes the core clock 4 MHz using the internal reference oscillator and MCGIRCLK 4 MHz
	
	// Set TPMs to use MCGIRCLK clock (selected as 4 MHz)
	SIM_BASE_PTR->SOPT2 |= SIM_SOPT2_TPMSRC(0b11);
	
	// Initialize global variables
	_transmit = false;
	_save = false;
	_sample = false;
	_cycleposition = 0;
	_eraseeeprom = false;
	_burnsequences = false;
	_releaseblades = false;
	_releasingtimer = 0;
	_bladesreleased = false;
	_pcrxbyteavailable = false;
	_pcrxbyte = 0;
	_externalBMPfaultcnt = 0;
	_externalBMPdead = false;
}

void Determine_State(int32_t *saved_initial_pressure)
{
	// Temporary initialization prior to actual implementation of determining state
	FSW_state = TST;
	_statetransition = true;

	if (ReadJumper(TEST_JUMPER) > 0) // Jumper is in place
	{
		// This is a good start -> begin mission normally
		FSW_state = TST;
	}
	else
	{
		// This is NOT a good start -> recover
#ifdef RTC_PRESENT
		FSW_state = (fsw_states)RTC_GetCurrentState();
		*saved_initial_pressure = RTC_GetInitialPressure();
		EEPROM_Address = RTC_GetEEPROMAddress();
		_minHX = RTC_GetMagnoXCal_Min(); _minHY = RTC_GetMagnoYCal_Min();
		_maxHX = RTC_GetMagnoXCal_Max(); _maxHY = RTC_GetMagnoYCal_Max();
#endif
	}
	_statetransition = true;
}

/* END Init functions */


/* BEGIN Scheduled functions */

void ComputePacketCheckSum()
{
	uint32_t sum = 0x00;
	int8_t i;
	for(i = 3; i < PACKET_LEN-1; i++){
		sum+=packet[i];
	}
    packet[CHK_SUM] = 0xFF - (sum & 0xFF);
}

void Transmit() // 39 ms
{
	// Update the FSW state field
	packet[FSW_STATE] = (uint8_t)FSW_state;
	
	// Update the packet count filed
	// Example 02 01
	uint16_t cnt = packet[PKT_CNT]; // cnt = 00 02
	cnt = cnt << 8; // cnt = 02 00
	cnt |= packet[PKT_CNT+1]; // cnt = 02 01
	cnt++; // cnt = 02 02
	
	ComputePacketCheckSum();
	Xbee_WriteArrayBytes(packet, PACKET_LEN);
	
	packet[PKT_CNT] = cnt >> 8;  // 00 02
	packet[PKT_CNT+1] = cnt & 0x00FF; // 02 02
}

void Save() // 14 ms
{
	// Form up the packet to save (this could be streamlined as per Amente's suggestion)
	// I'm leaving that to you Maytham if you'd like to..
	packet[FSW_STATE] = (uint8_t)FSW_state;

	// Process the save
	AT_Writes(&EEPROM_Address, packet+17, PACKET_LEN-18); // start at the packet TeamID index, and write everything after except the checksum
}

sample_data Sample() // 33 ms
{
	// New parallel sampling structure 
	sample_data temp;
	
	// Ask for temperature from BMPs
	askTemperature1();
#ifdef TWO_BMPS
	if (!_externalBMPdead) { askTemperature2(); }
#endif
	
	// Start the 5ms timer for BMP temperature conversion
	StartTimer(5);
	
	// Get the magnetometer data
	temp.MAGdata = Get_Magnetometer(magcaldata);
	
	// Get the RTC data
#ifdef RTC_PRESENT
	temp.RTCdata = RTC_GetData();
#endif
	
	// Make sure timer times out
	WaitForTimer();
	
	// Retrieve temperature from BMPs
	int16_t temperature1 = receiveTemperature1();
#ifdef TWO_BMPS
	int16_t temperature2 = 0;
	if (!_externalBMPdead) { temperature2 = receiveTemperature2(); }
#endif
	
	// Ask for pressure from BMPs
	askPressure1();
#ifdef TWO_BMPS
	if (!_externalBMPdead) { askPressure2(); }
#endif
	
	// Start the 5ms timer for BMP pressure conversion
	StartTimer(5);
	
	// Get the accelerometer data
	temp.ACCdata = MPU_Get_Accel();
	
	// Read the battery voltage
	temp.BattVolts = ReadBatteryVoltage();
	
	// Make sure timer times out
	WaitForTimer();
	
	// Retrieve pressure from BMPs
	int32_t pressure1 = receivePressure1();
#ifdef TWO_BMPS
	int32_t pressure2 = 0;
	if (!_externalBMPdead) { pressure2 = receivePressure2(); }
#endif
	
	// Perform BMP calculations
	temp.BMPdata_internal = CalculateBMP(bmp1caldata, temperature1, pressure1);
#ifdef TWO_BMPS
	if (temperature2 == 0 && pressure2 == 0) _externalBMPfaultcnt++;
	else _externalBMPdead = false;
	if (_externalBMPfaultcnt > SENSOR_FAILURE_ARB) { _externalBMPdead = true; }
	if (!_externalBMPdead) { temp.BMPdata_external = CalculateBMP(bmp2caldata, temperature2, pressure2); }
	else { temp.BMPdata_external = CalculateBMP(bmp1caldata, temperature1, pressure1); }
#endif
	
	// Put BMP data in the global packet
	packet[INT_TEMP] =  (uint8_t) (temp.BMPdata_internal.temperature >> 8);
	packet[INT_TEMP+1] =  (uint8_t) (temp.BMPdata_internal.temperature & 0x00FF); 
#ifdef TWO_BMPS
	packet[EXT_TEMP] =  (uint8_t) (temp.BMPdata_external.temperature >> 8);
	packet[EXT_TEMP+1] =  (uint8_t) (temp.BMPdata_external.temperature & 0x00FF);
#endif
	
	// Put the accel data in the global packet
	packet[ACCEL_X] =  (uint8_t) (temp.ACCdata.AccelX >> 8);
	packet[ACCEL_X+1] =  (uint8_t) (temp.ACCdata.AccelX & 0x00FF); 
	packet[ACCEL_Y] =  (uint8_t) (temp.ACCdata.AccelY >> 8);
	packet[ACCEL_Y+1] =  (uint8_t) (temp.ACCdata.AccelY & 0x00FF);
	packet[ACCEL_Z] =  (uint8_t) (temp.ACCdata.AccelZ >> 8);
	packet[ACCEL_Z+1] =  (uint8_t) (temp.ACCdata.AccelZ & 0x00FF);
	
	// Put the battery voltage in the global packet
	packet[VOLTAGE] = (uint8_t) (temp.BattVolts >> 8);
	packet[VOLTAGE+1] = (uint8_t) (temp.BattVolts & 0x00FF);
	
	// Put the RTC data in the global packet
#ifdef RTC_PRESENT
	int missiontime;
	missiontime = temp.RTCdata.hours*60*60 + temp.RTCdata.minutes*60 + temp.RTCdata.seconds; // mission time is in seconds
	packet[MISSION_TIME_IDX] = (uint8_t) (missiontime >> 8);
	packet[MISSION_TIME_IDX+1] = (uint8_t) (missiontime & 0x00FF);
#endif
	
	return temp;
}

/* END Scheduled functions */


/* BEGIN General functions */

int32_t CalibrateZeroAltitude(int16_t* altitudearray, uint8_t altitudearraylength)
{
	int32_t initialpressure;
	initialpressure = getInitialPressure1(bmp1caldata);
	// Set altitude array to all zeroes (i.e. no altitude gain)
	int i;
	for (i = 0; i < altitudearraylength; i++)
	{
		altitudearray[i] = 0;
	}
	return initialpressure;
}

int16_t MedianFilterAltitude(int16_t* altitudearray, uint8_t altitudearraylength, uint8_t currentpos, uint8_t filterlength)
{
	int16_t p_buf[5];
	// Copy the values to be filtered into the dynamically allocated array
	int i;
	for (i = 0; i < 5; i++) // only the last filterlength elements...
	{
		if (currentpos - i >= 0) // handling wraparound!!
			{ p_buf[i] = altitudearray[currentpos - i]; }
		else
			{ p_buf[i] = altitudearray[altitudearraylength - i]; }
	}
	// Optimized median calculation for five values
	P_SORT(p_buf[0],p_buf[1]) ; P_SORT(p_buf[3],p_buf[4]) ; P_SORT(p_buf[0],p_buf[3]) ;
	P_SORT(p_buf[1],p_buf[4]) ; P_SORT(p_buf[1],p_buf[2]) ; P_SORT(p_buf[2],p_buf[3]) ;
	P_SORT(p_buf[1],p_buf[2]) ;
	// Return the middle eliminate
	return p_buf[2];
#ifdef LEGACY
	int16_t* tofilter = calloc(filterlength, sizeof(int16_t)); // Dynamically allocate an array of size filterlength
	if (tofilter == NULL)
		return altitudearray[0];
	int16_t result;
	// Copy the values to be filtered into the dynamically allocated array
	int i, j; int16_t a;
	for (i = 0; i < filterlength; i++) // only the last filterlength elements...
	{
		if (currentpos - i >= 0) // handling wraparound!!
			tofilter[i] = altitudearray[currentpos - i];
		else
			tofilter[i] = altitudearray[altitudearraylength - i];
	}
	// Sort the values into ascending order
	for (i = 0; i < filterlength; ++i)
	{
		for (j = i + 1; j < filterlength; ++j)
		{
			if (tofilter[i] > tofilter[j]) // values out of order, swap!
			{
				a =  tofilter[i];
				tofilter[i] = tofilter[j];
				tofilter[j] = a;
			}
		}
	}
	result = tofilter[filterlength/2]; // Truncates for even filterlength (avoid having an even filterlength value)
	free(tofilter); // Free the dynamically allocated memory
	return result;
#endif
}

AK8963_Data AverageMagData(AK8963_Data* magdatas, uint8_t numberofsamples)
{
	if (numberofsamples == 1) // If only collecting one sample retun immediately
		{ return magdatas[0]; }
	int hx = 0; int hy = 0; int hz = 0; AK8963_Data result;
	uint8_t averages = numberofsamples;
	uint8_t i;
	for (i = 0; i<numberofsamples; i++)
	{
		if(magdatas[i].Success == 0) // Only average successful samples
			{ averages--; }
		else
		{
			// Add values to totals
			hx += magdatas[i].HX;
			hy += magdatas[i].HY;
			hz += magdatas[i].HZ;
		}
	}
	if (averages > 0)
	{
		// Divide by number of averages (excludes failed samples)
		result.HX = (int16_t)(hx/averages);
		result.HY = (int16_t)(hy/averages);
		result.HZ = (int16_t)(hz/averages);
		result.Success = 0xFF;
	}
	else
	{
		result = magdatas[0]; // Failed result (will return all zeroes)
	}
	return result;
}

MPU6500_Gyro AverageGyroData(MPU6500_Gyro* gyrodatas, uint8_t numberofsamples)
{
	if(numberofsamples == 1) // If only collecting one sample retun immediately
		{ return gyrodatas[0]; }
	int gx = 0; int gy = 0; int gz = 0; MPU6500_Gyro result;
	uint8_t i;
	for (i = 0; i<numberofsamples; i++)
	{
		gx += gyrodatas[i].GyroX;
		gy += gyrodatas[i].GyroY;
		gz += gyrodatas[i].GyroZ;
	}
	result.GyroX = (int16_t)(gx/numberofsamples);
	result.GyroY = (int16_t)(gy/numberofsamples);
	result.GyroZ = (int16_t)(gz/numberofsamples);
	return result;
	return gyrodatas[0];
}

uint16_t DetermineDescentDistance(int16_t* HistoricAltitudeData, uint8_t alt_buffer_len, uint8_t current_altitude_pos, int samplesback)
{
	if (current_altitude_pos - samplesback < 0) // Handle circular buffer wrap around
	{
		return HistoricAltitudeData[alt_buffer_len - (samplesback - current_altitude_pos)] - HistoricAltitudeData[current_altitude_pos];
	} else { // Simple case
		return HistoricAltitudeData[current_altitude_pos - samplesback] - HistoricAltitudeData[current_altitude_pos];
	}
}

void EraseEEPROM()
{
#ifdef EEPROM_PRESENT
	AT_Clear();
#endif
	_eraseeeprom = false;
}

void UpdateStateEEPROM(fsw_states state)
{
	RTC_SaveCurrentState((uint8_t)state);
	
#ifdef DEBUG
	switch(state)
	{
	case LNC:
		PC_WriteString("Launch State");
		break;
	case ASC:
		PC_WriteString("Ascent State");
		break;
	case PDS:
		PC_WriteString("Pre-Deployment State");
		break;
	case SVD:
		PC_WriteString("Science Vehicle Descent State");
		break;
	case LND:
		PC_WriteString("Landed State");
		break;
	default:
		PC_WriteString("Critical Error");
		break;
	}
#endif
}

void ServicePC()
{
	// If we receive a byte from the PC
	if (_pcrxbyteavailable)
	{
#ifdef DEBUG
		ReadAddress=0;
		int debuggingCount=0;
		for(;debuggingCount<30;debuggingCount++){
			Sample();
			Transmit();
			Save();
			delay(5);
		}
#endif
		if (_pcrxbyte == EEPROM_REQUEST_KEY) // PC wants the EEPROM data
		{
			Light(1); Light(2); Light(3); Light(4);
			uint8_t eepromdata[256];
			while(AT_Reads(&ReadAddress, &EEPROM_Address, eepromdata)) // Read until AT_Reads signals all data has been read
			{
				PC_WriteArrayBytes(eepromdata, 256); // Write out the eeprom data to the PC
			}
			Darken(1); Darken(2); Darken(3); Darken(4);
		}
#ifdef RELEASE_TESTING
		else if (_pcrxbyte == 97) // a in ascii
		{
			// Simulated release
			_statetransition = true;
			bool amente = false;
			while(!amente)
			{
				// Begin release sequence
				// Release sequence - stage 1 (container release)
				if (_statetransition)
					{ LightAll(); ReleaseFromContainer(); _burnsequences = true; }
				
				_statetransition = false;
				
				// Release sequence - stage 2 (blade deployment)
				if (_releaseblades) // Counter in interrupt handler times releasess
				{
					StopReleaseFromContainer(); // Lower the container release line
					ReleaseBlades();
					_bladesreleased = true;
				}
				_releaseblades = false; // The interrupt handler should not set _releaseblades true bc _bladesreleased is now true
				
				// Lower all release lines for safety
				if (!_burnsequences)
					{ StopReleaseBlades(); DarkenAll(); amente = true; Light(2); Light(7); }
			}
			
		}
#endif
		else if (_pcrxbyte == CALIBRATION_COMMAND) // c in ascii
		{
			// Begin calibration sequence
			_calinprogress = true;
			_caltimer = 0; bool firsttime = true;
			if (FSW_state == TST) { Darken(4); Darken(5); }
			Light(1); Light(8);
			while (_calinprogress)
			{
				AK8963_Data magdata = Get_Magnetometer(magcaldata);
				if (firsttime) // if first time save the values
				{
					_maxHX = magdata.HX; _minHX = magdata.HX;
					_maxHY = magdata.HY; _minHY = magdata.HY;
					firsttime = false;
				}
				else
				{
					// if over or under max/min respectively, save it
					if (magdata.HX < _minHX)
					{ _minHX = magdata.HX; }
					else if (magdata.HX > _maxHX)
					{ _maxHX = magdata.HX; }
					if (magdata.HY < _minHY)
					{ _minHY = magdata.HY; }
					else if (magdata.HY > _maxHY)
					{ _maxHY = magdata.HY; }
				}
			}
			if (FSW_state == TST) { Light(4); Light(5); }
			Darken(1); Darken(8);
			// Save the values in RTC EEPROM
			RTC_SaveMagnoXCal_Min(_minHX); RTC_SaveMagnoYCal_Min(_minHY);
			RTC_SaveMagnoXCal_Max(_maxHX); RTC_SaveMagnoYCal_Max(_maxHY);
		}
		_pcrxbyteavailable = false;
	}
}

/* END General functions */


/* BEGIN Main function */

int main(void)
{
	/* BEGIN Main variable declarations */
	
	/* BMP180 data structures */
	int32_t initial_pressure; // Initial pressure to be read upon power up and saved
	int16_t HistoricAltitudeData[ALTITUDE_MEMORY_LEN]; uint8_t current_altitude_pos = 0; // Before processed by median filter
	
	/* State transition assist variables */
	uint32_t Max_Altitude = 0; // Maximum altitude seen
	
	/* PID motor control variables */
	//int motordrive = 0; // The current drive of the motor - in the range (-100, 100)
#if MAG_STABILIZE_SAMPLES != 0
	int mag_i_term = 0; // The value of the last integral term from the magnetometer PID control system
	int mag_d_term = 0; // The value of the last derivative term from the magnetometer PID control system
	int magerrorval = 0;
#endif
	int magcorrection = 0; // Will always be 0 if mag is not used to stabilize
#if GYRO_STABILIZE_SAMPLES != 0
	int gyro_i_term = 0; // The value of the last integral term from the gyroscope PID control system
	int gyro_d_term = 0; // The value of the last derivative term from the gyroscope PID control system
#endif
	int gyrocorrection = 0; // Will always be 0 if gyro is not used to stabilize
	
	// SampleData holder
	sample_data SampleData;
	
	// Altitude Variable
	int16_t altitude;
	
	/* END Main variable declarations */
	
	
	/* BEGIN Initialization sequence */
	
	// Basic chip initialization (clock setup, variable initializations, ... )
	Basic_Init();
	
	// Initialize the GPIO and LEDs
	Init_GPIO(); Init_LEDs(); Init_Jumpers();
	
	// Flash all LEDs for a second to indicate powerup 
	LightAll();
	delay(1000);
	
	// Turn cal LEDs off
	Darken(1); Darken(8);
	
	// Initialize the MotorControl
	Init_MotorControl();
	
	// Initialize the ADC
	adc_init();
	
	// Initialize the I2C ports to be used
	I2C0_init();
#ifdef TWO_BMPS
	I2C1_init();
#endif
	
	// Determine start state and set initial pressure, if applicable (must be done after Init_Jumpers)
	Determine_State(&initial_pressure);
	
	// Initialize the sensors (must be done after initializing the I2C ports)
	bmp1caldata = BMP1801_init();
#ifdef TWO_BMPS
	bmp2caldata = BMP1802_init();
#endif
	MPU_Select_ByPass(); // Set mpu6500 to bypass mode to allow reading of magnetometer
	magcaldata = GetMagnoCalibrationData(); // Get the magnetometer calibration data
	Magno_SetContinuous(); // Set the magnetometer to continuous mode
#ifdef RTC_PRESENT
	//RTC_SelectMode(); // Maytham says not necessary
	RTC_SetControl();
#endif
	Darken(2); Darken(7);
	if (FSW_state != TST) { DarkenAll(); }
	
	// Initialize the UART ports to be used
	Xbee_Init(XBEE_BAUD); // UART0
	PC_Init(PC_BAUD); // UART2
	
	// Perform the first sample so our first transmit is good
	Sample();
	
	// Initialize the scheduling of periodic events (period specified at top of source; can not be changed)
	Init_Servicing(SERVICE_T);
	
	/* END Initialization sequence */
	
	
	/* BEGIN Testbench area */
	// THIS MUST BE COMMENTED OUT OR EMPTY FOR ANY DELIVERY
#ifdef DEBUG
	
	/* Timing Tests
	uint32_t Mask_4 = 1<<4;
	
	SIM_BASE_PTR->SCGC5 |= 1<<12; // Port clock to port D
	
	PORTD_PCR4 |= 1<<8; // Set to GPIO (JP6 low pin)
	
	GPIOD_PDDR |= Mask_4; // Set to output
	
	GPIOD_PCOR |= Mask_4; // Set high
	
	for(;;) 
	{
		sample_data temp;
		temp = Sample();
		GPIOD_PSOR |= Mask_4;
		Transmit();
		GPIOD_PCOR |= Mask_4;
		delay(200);
	}*/
	
	//FSW_state = SVD;
	
#endif
	/* END Testbench area */
	
	
	/* BEGIN Main loop */
	
	for(;;)
	{
		// Perform serviced processes
		if (_transmit) // If transmit duration has passed...
		{
			Transmit();
			_transmit = false;
		}
		if (_save) // If a save duration has passed...
		{
#ifdef EEPROM_PRESENT
			Save();
#endif
#ifdef RTC_PRESENT
			RTC_SaveEEPROMAddress(EEPROM_Address);
#endif
			_save = false;
		}
		if (_sample) // If a sample duration has passed...
		{
			// Sample the sensors
			SampleData = Sample();
			_sample = false;

			// Compute altitude and store it
			altitude = getAltitude(SampleData.BMPdata_internal, initial_pressure);
			current_altitude_pos++;
			if (current_altitude_pos == ALTITUDE_MEMORY_LEN) current_altitude_pos = 0;
			HistoricAltitudeData[current_altitude_pos] = altitude;
			altitude = MedianFilterAltitude(HistoricAltitudeData, ALTITUDE_MEMORY_LEN, current_altitude_pos, ALTITUDE_MEDIAN_LEN);
			
			// Put the new value of the altitude in the packet
			packet[ALT_IDX] =  (uint8_t) (altitude >> 8);
			packet[ALT_IDX+1] =  (uint8_t) (altitude & 0xFF); 
		}
		
		
		// Perform state specific processes and check state exit conditions
		switch(FSW_state)
		{
			/** TeSTing State Handling **/
			case TST:
				// Calibrate zero altitude here
				if (_statetransition)
				{
#ifdef DEBUG
					PC_WriteString("Testing State");
#endif
					initial_pressure = CalibrateZeroAltitude(HistoricAltitudeData, ALTITUDE_MEMORY_LEN);
					RTC_SaveInitialPressure(initial_pressure);
					Darken(3); Darken(6);
				}
				_statetransition = false;
				
				// Check to see if the PC has sent data
				ServicePC();
				
				// Check exit condition and, if exiting, set _eraseeeprom to true
				if (!(ReadJumper(TEST_JUMPER) > 0)) // TEST_JUMPER has been removed
				{ FSW_state = LNC; _eraseeeprom = true; _statetransition = true; 
#ifdef JUMPER_TO_SVD
				FSW_state = SVD;
#endif
				}
				
				// If jumper 3 is placed on, perform calibration routine
				if(ReadJumper(3) > 0)
				{
					// Begin calibration sequence
					_calinprogress = true;
					_caltimer = 0; bool firsttime = true;
					if (FSW_state == TST) { Darken(4); Darken(5); }
					Light(1); Light(8);
					while (_calinprogress)
					{
						AK8963_Data magdata = Get_Magnetometer(magcaldata);
						if (firsttime) // if first time save the values
						{
							_maxHX = magdata.HX; _minHX = magdata.HX;
							_maxHY = magdata.HY; _minHY = magdata.HY;
							firsttime = false;
						}
						else
						{
							// if over or under max/min respectively, save it
							if (magdata.HX < _minHX)
							{ _minHX = magdata.HX; }
							else if (magdata.HX > _maxHX)
							{ _maxHX = magdata.HX; }
							if (magdata.HY < _minHY)
							{ _minHY = magdata.HY; }
							else if (magdata.HY > _maxHY)
							{ _maxHY = magdata.HY; }
						}
					}
					if (FSW_state == TST) { Light(4); Light(5); }
					Darken(1); Darken(8);
					// Save the values in RTC EEPROM
					RTC_SaveMagnoXCal_Min(_minHX); RTC_SaveMagnoYCal_Min(_minHY);
					RTC_SaveMagnoXCal_Max(_maxHX); RTC_SaveMagnoYCal_Max(_maxHY);
				}
				
				break;
				
				
			/** LauNCh State Handling **/
			case LNC:
				// *ONLY* if exiting TST state do we erase EEPROM and set mission time to 0
				if (_eraseeeprom)
				{
					EraseEEPROM();
#ifdef RTC_PRESENT
					RTC_SetTime(0,0,0);
#endif
					Darken(4); Darken(5);
				}
				
				// If _statetransition, update state in EEPROM
				if (_statetransition)
					UpdateStateEEPROM(LNC);
				_statetransition = false;
				
				int16_t lnc_mfaltitude = MedianFilterAltitude(HistoricAltitudeData, ALTITUDE_MEMORY_LEN, current_altitude_pos, ALTITUDE_MEDIAN_LEN);
				// Keep track of max altitude seen
				if (lnc_mfaltitude > Max_Altitude)
					{ Max_Altitude = lnc_mfaltitude; }
				
				// Check exit condition:
				if (lnc_mfaltitude > LNC_EXIT_ALTITUDE)
					{ FSW_state = ASC; _statetransition = true; }
				
				break;
				
				
			/** ASCent State Handling **/
			case ASC:
				// If _statetransition, update state in EEPROM
				if (_statetransition)
					{ UpdateStateEEPROM(ASC); }
				_statetransition = false;
				
				int16_t asc_mfaltitude = MedianFilterAltitude(HistoricAltitudeData, ALTITUDE_MEMORY_LEN, current_altitude_pos, ALTITUDE_MEDIAN_LEN);
				// Keep track of max altitude seen
				if (asc_mfaltitude > Max_Altitude)
					{ Max_Altitude = asc_mfaltitude; }
				
				// Check state exit condition:
				if (Max_Altitude - asc_mfaltitude > ASC_EXIT_BELOW_APOGEE)
				{ FSW_state = PDS; _statetransition = true; }
				
				break;
				
				
			/** Pre-Deployment of Scienc vehicle State Handling **/
			case PDS:
				// If _statetransition, update state in EEPROM
				if (_statetransition)
					{ UpdateStateEEPROM(PDS); }
				_statetransition = false;
				
				// Check state exit condition:
				if (MedianFilterAltitude(HistoricAltitudeData, ALTITUDE_MEMORY_LEN, current_altitude_pos, ALTITUDE_MEDIAN_LEN) < PDS_EXIT_ALTITUDE)
					{ FSW_state = SVD; _statetransition = true; }
				
				break;
				
				
			/** Science Vehicle Descent State Handling **/
			case SVD:
				// If _statetransition, update state in EEPROM
				if (_statetransition)
					{ UpdateStateEEPROM(SVD); }
				
				// Begin release sequence
				// Release sequence - stage 1 (container release)
				if (_statetransition)
					{ ReleaseFromContainer(); _burnsequences = true; }
				
				_statetransition = false;
				
				// Release sequence - stage 2 (blade deployment)
				if (_releaseblades) // Counter in interrupt handler times releasess
				{
					StopReleaseFromContainer(); // Lower the container release line
					ReleaseBlades();
					_bladesreleased = true;
				}
				_releaseblades = false; // The interrupt handler should not set _releaseblades true bc _bladesreleased is now true
				
				// Lower all release lines for safety
				if (!_burnsequences)
					{ StopReleaseBlades(); }
				
				// Extra gyroscop/magnetometer sampling and stabilisation code go here
				// Only start stabilistation after burn sequences
				if (!_burnsequences)
				{
					/* Magnetometer PID Section BEGIN */
					magcorrection = 0;
#if MAG_STABILIZE_SAMPLES != 0
					// Average MAG_STABILIZE_SAMPLES samples from the magnetometer
					AK8963_Data magdatas[MAG_STABILIZE_SAMPLES];
					uint8_t i;
					for (i = 0; i<MAG_STABILIZE_SAMPLES; i++)
						{ magdatas[i] = Get_Magnetometer(magcaldata); }
					AK8963_Data magaverage = AverageMagData(magdatas, MAG_STABILIZE_SAMPLES);
					// Correct the mag values with the reads from the calibration procedure
					magaverage.HX -= ((float)_maxHX + (float)_minHX)/2.0;
					magaverage.HY -= ((float)_maxHY + (float)_minHY)/2.0;
					
					// Calculate the error value from the magnetometer data - outputs in the range (-180, 180)
					magerrorval = Calculate_Error_Mag(magaverage);
					
					// Calculate the correction value for the motor
					magcorrection = Calculate_Correction(magerrorval, &mag_i_term, &mag_d_term, KP_MAG, KI_MAG, KD_MAG);
#endif
					/* Magnetometer PID Section END */
					
#ifdef DEB
					char buffer[80];
					sprintf(buffer, "x: %d	", magaverage.HX);
					PC_WriteString(buffer);
					sprintf(buffer, "y: %d	", magaverage.HY);
					PC_WriteString(buffer);
					sprintf(buffer, "error: %d	", magerrorval);
					PC_WriteString(buffer);
					sprintf(buffer, "correction: %d", magcorrection);
					PC_WriteString(buffer);
					PC_WriteString("\n");
#endif
					
					/* Gyroscope PID Section BEGIN */
					gyrocorrection = 0;
#if GYRO_STABILIZE_SAMPLES != 0
					// Average GYRO_STABILIZE_SAMPLES samples from the gryoscope
					MPU6500_Gyro gyrodatas[GYRO_STABILIZE_SAMPLES];
					uint8_t j;
					for (j = 0; j<GYRO_STABILIZE_SAMPLES; j++)
							{ gyrodatas[j] = MPU_Get_Gyro(); }
					MPU6500_Gyro gyroaverage = AverageGyroData(gyrodatas, GYRO_STABILIZE_SAMPLES);
					
					// Error value is just the z componenet of the gyroscope average - that is, the rotational acceleration about the z axis
					// Calculate the correction value for the motor
					gyrocorrection = Calculate_Correction(gyroaverage.GyroZ, &gyro_i_term, &gyro_d_term, KP_GYRO, KI_GYRO, KD_GYRO);
#endif
					/* Gyroscope PID Section END */
					
					// Determine value to set for motor
#if MAG_STABILIZE_SAMPLES == 0 && GYRO_STABILIZE_SAMPLES == 0
# error need to sample something to stabilize
#endif
					int setmotor = -1 * MAG_FACTOR * magcorrection - GYRO_FACTOR * gyrocorrection;
					if (setmotor > 100) { setmotor = 100; }
					else if (setmotor < -100) { setmotor = -100; }
					if (setmotor == 0)
						{ Motor_Stop(); }
					else if (setmotor > 0)
						{ Motor_FWD(setmotor); }
					else if (setmotor < 0)
						{ Motor_BWD(-1*setmotor); } // Parameter must be a positive value
				} //end if(!_burnsequences)
#ifdef cats
				// Check state exit condition:
				if (DetermineDescentDistance(HistoricAltitudeData, ALTITUDE_MEMORY_LEN, current_altitude_pos, SVD_EXIT_SAMPLES_CONSTANT) < SVD_EXIT_DEADBAND)
				{ FSW_state = LND; _statetransition = true; }
#endif
				break;
				
				
			/** LaNDed State Handling **/
			case LND:
				// If _statetransition, update state in EEPROM
				if (_statetransition)
					{ UpdateStateEEPROM(LND); }
				
				// Activate buzzer
				if (_statetransition)
					{ ActivateLocationAssistanceDevice(); }
				
				_statetransition = false;
				
				// Check to see if the PC has sent data
				ServicePC();
				
				// It is not possible to exit this state -> this is the end state
				break;
				
			default:
				// Raise error and recover
				Determine_State(&initial_pressure);
				break;
		}
	}
	
	/* END Main loop */
	
}

/* END Main function */

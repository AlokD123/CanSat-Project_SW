/*
 * This file will be the main loop for the RavenKnights 2015.
 * This code is developped for our custom board running an MKL16Z.
 *
 * Contributors: Erik, Maytham, Alok
 * 
 * Current todos:
 * ***** Test the PID control system for stabilization
 * ****  Test the transmission
 * ****  Test the state transitions
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
#include "stdio.h" // include the code for string manipulations
#include "stdlib.h" // include the code for dynamic allocations

/* END Include external code */

/* BEGIN Pre-processor directives for configuration */

#define DEBUG

#define TEAM_ID 1001

#define SERVICE_T 100 // Period for servicing interrupt in ms

// Memory length def'ns
#define SENSOR_DATA_MEMORY_LEN 10 // Number of sensor samples to save in RAM
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
#define BLADE_RELEASE_DELAY 0.5 // The number of seconds to wait after deploying from container before releasing the blades
// Note: BLADE_RELEASE_DELAY also defines the amount of time to keep the container release line high
#define BLADE_RELEASE_TIME 0.5 // The number of seconds to keep the blade release line high

// Stabilization def'ns
// Setting either to 0 entirely disables all associated stabilization code
#define MAG_STABILIZE_SAMPLES 3 // The number of magnetometer samples to average for stabilization
#define GYRO_STABILIZE_SAMPLES 0 // The number of gyroscope samples to average for stabilization

// PID def'ns
/* Tuning factors for magnetometer PID control */
#define KP_MAG 0.01 // The value for the proportional gain for the mag PID
#define KI_MAG 0.01 // The value for the integral gain for the mag PID
#define KD_MAG 0.0001 // The value for the derivative gain for the mag PID
/* Tuning factors for gyroscope PID control */
#define KP_GYRO 0.01 // The value for the proportional gain for the gyro PID
#define KI_GYRO 0.01 // The value for the integral gain for the gyro PID
#define KD_GYRO 0.0001 // The value for the derivative gain for the gyro PID

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
	//MPU6500_Gyro GYRdata; // Not needed for transmitting/saving... sample only when needed
	AK8963_Data MAGdata;
	//BMP180 BMPdata_external;
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
// Event set flags
bool _statetransition; // Signals we have just transitioned in states
bool _eraseeeprom; // Signals we have left the testing state and should erase the eeprom
bool _burnsequences; // Signals we are performing the burn sequences
bool _releaseblades; // Signals we should release the CanSat blades
/* Other globals */
uint8_t _cycleposition; // Keeps track of where we are in the time slices
uint8_t _releasingtimer; // Times the two releasing sequences using the Interrup_Service_TPM1

/* END Global variable declarations */

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
			_transmit = true;
		else
			_transmit = false;
		break;
	case 1:
		// Sample
		_sample = true;
		break;
	case 2:
		// Save
		if (FSW_state != TST) // Do not save in testing state
			_save = true;
		else
			_save = false;
		break;
	case 3:
		// Sample
		_sample = true;
		break;
	case 4:
		// Save
		if (FSW_state != TST) // Do not save in testing state
			_save = true;
		else
			_save = false;
		break;
	case 5:
		// Sample
		_sample = true;
		break;
	case 6:
		// Save
		if (FSW_state != TST) // Do not save in testing state
			_save = true;
		else
			_save = false;
		break;
	case 7:
		// Sample
		_sample = true;
		break;
	case 8:
		// Save
		if (FSW_state != TST) // Do not save in testing state
			_save = true;
		else
			_save = false;
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
		_cycleposition = 0;
	else
		_cycleposition++;
	
	// Blade release timing
	if (_burnsequences)
	{
		_releasingtimer++;
		if (_releasingtimer >= (float)BLADE_RELEASE_DELAY*1000.0/(float)SERVICE_T)
			_releaseblades = true;
		
		if (_releasingtimer >= (float)BLADE_RELEASE_DELAY*1000.0/(float)SERVICE_T + (float)BLADE_RELEASE_TIME*1000.0/(float)SERVICE_T)
			_burnsequences = false;
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
	
	// Initialize variables
	_transmit = false;
	_save = false;
	_sample = false;
	_cycleposition = 0;
	_eraseeeprom = false;
	_burnsequences = false;
	_releaseblades = false;
	_releasingtimer = false;
}

void Determine_State(int32_t* saved_initial_pressure) // To be implemented*** (Erik/Maytham)
{
	// Temporary initialization prior to actual implementation of determining state
	FSW_state = TST;
	_statetransition = true;
	/*
	 * To do in this function: ***
	 * Recover state
	 * Recover value of initial pressure
	 */
}

/* END Init functions */

/* BEGIN Scheduled functions */

/*struct transmitnode {
	uint8_t data;
	struct transmitlist* ptr;
};
typedef struct transmitnode TransmitNode;

TransmitNode* ConstructNode (uint8_t data, TransmitNode* next)
{
	TransmitNode* rtn = malloc(sizeof(TransmitNode));
	rtn->data = data;
	rtn->ptr = next;
	return rtn;
}*/

uint32_t ComputeBytesum(uint8_t* array, int elements)
{
	int i; uint32_t rtn = 0;
	for(i = 0; i < elements; i++)
	{
		rtn += array[i];
	}
	return rtn;
}

void Transmit(uint8_t *packetcount, int16_t altitude) // To be implemented*** (Erik/Alok)
{
	// Declare the array that will hold the bytes to transmit.
	uint8_t transmit[50]; int index = 0; int i = 0; // number needs to be changed to correct value*****
	
	/** Start delimiter **/
	//Xbee_WriteByte(0x7E);
	transmit[index++] = 0x7E;
	
	/** Length in bytes **/
	transmit[index++] = 0; transmit[index++] = 36; // Not including the checksum
	
	/** API ID / Frame type **/
	//Xbee_WriteByte(0x10);
	transmit[index++] = 0x10;
	
	/** API Frame ID **/
	//Xbee_WriteByte(*packetcount);
	transmit[index++] = *packetcount;
	
	/** 64 bit address **/
	uint8_t dest64[8] = {0x0, 0x13, 0xA2, 0x0, 0x40, 0xC0, 0xC5, 0xF1};
	//Xbee_WriteArrayBytes(dest64, 8);
	for(i = 0; i < 8; i++)
	{
		transmit[index++] = dest64[i];
	}
	
	/** 16 bit address **/
	uint8_t dest16[2] = {0xFF, 0xFE};
	//Xbee_WriteArrayBytes(dest16, 2);
	for(i = 0; i < 2; i++)
	{
		transmit[index++] = dest16[i];
	}
	
	/** Broadcast byte **/
	//Xbee_WriteByte(0x0); // Broadcast radius byte
	transmit[index++] = 0x0; // Broadcast radius byte
	
	/** Option byte **/
	//Xbee_WriteByte(0x0); // Option byte
	transmit[index++] = 0x0; // Option byte
	
	/** Team ID - 2 bytes **/
	uint16_t teamid = TEAM_ID;
	uint8_t teamidl = (uint8_t)(teamid | 0xFF);
	teamid >>= 8;
	uint8_t teamidh = (uint8_t)(teamid | 0xFF);
	//Xbee_WriteByte(teamidh); Xbee_WriteByte(teamidl);
	transmit[index++] = teamidh; transmit[index++] = teamidl;
	
	/** Mission time - 2 bytes **/
	transmit[index++] = 0x0; transmit[index++] = 0x0;
	
	/** Altitude - 2 bytes **/
	uint8_t altitudel = (uint8_t)(altitude | 0xFF);
	altitude >>= 8;
	uint8_t altitudeh = (uint8_t)(altitude | 0xFF);
	//Xbee_WriteByte(altitudeh); Xbee_WriteByte(altitudel);
	transmit[index++] = altitudeh; transmit[index++] = altitudel;
	
	/** Outside air temperature - 4 bytes **/
	transmit[index++] = 0x0; transmit[index++] = 0x0;
	transmit[index++] = 0x0; transmit[index++] = 0x0;
	
	/** Internal air temperature - 4 bytes **/
	transmit[index++] = 0x0; transmit[index++] = 0x0;
	transmit[index++] = 0x0; transmit[index++] = 0x0;
	
	/** FSW state - 1 bytes **/
	transmit[index++] = 0x0;
	
	/** Power supply voltage - 3 bytes **/
	transmit[index++] = 0x0; transmit[index++] = 0x0; // . in ascii
	transmit[index++] = 0x0;
	
	/** Accelerometer data - 2 bytes **/
	transmit[index++] = 0x0; transmit[index++] = 0x0;
	
	/** Packet count - 2 bytes **/
	*packetcount = *packetcount + 1;
	//Xbee_WriteByte(0x0); Xbee_WriteByte(*packetcount);
	transmit[index++] = 0x0; transmit[index++] = *packetcount;
	
	/** Checksum **/
	uint32_t bytesum = ComputeBytesum(transmit, index + 1);
	uint8_t checksum = 0xFF - (bytesum & 0xFF);
	transmit[index++] = checksum;
	
	Xbee_WriteArrayBytes(transmit, index + 1);
	//PC_WriteArrayBytes(transmit, index + 1);
	//PC_WriteString("Transmission sent.");
}

void Transmit2(uint8_t *packetcount, int16_t altitude)
{
	// Declare the array that will hold the bytes to transmit.
	uint8_t transmit[19]; int index = 0; int i = 0; // number needs to be changed to correct value*****
	
	/** Start delimiter **/
	//Xbee_WriteByte(0x7E);
	transmit[index++] = 0x7E;
	
	/** Length in bytes **/
	transmit[index++] = 0; transmit[index++] = 15; // not including checksum
	
	/** API ID / Frame type **/
	//Xbee_WriteByte(0x10);
	transmit[index++] = 0x10;
	
	/** API Frame ID **/
	//Xbee_WriteByte(*packetcount);
	transmit[index++] = *packetcount;
	
	/** 64 bit address **/
	uint8_t dest64[8] = {0x0, 0x13, 0xA2, 0x0, 0x40, 0xC0, 0xC5, 0xF1};
	//Xbee_WriteArrayBytes(dest64, 8);
	for(i = 0; i < 8; i++)
	{
		transmit[index++] = dest64[i];
	}
	
	/** 16 bit address **/
	uint8_t dest16[2] = {0xFF, 0xFE};
	//Xbee_WriteArrayBytes(dest16, 2);
	for(i = 0; i < 2; i++)
	{
		transmit[index++] = dest16[i];
	}
	
	/** Broadcast byte **/
	//Xbee_WriteByte(0x0); // Broadcast radius byte
	transmit[index++] = 0x0; // Broadcast radius byte
	
	/** Option byte **/
	//Xbee_WriteByte(0x0); // Option byte
	transmit[index++] = 0x0; // Option byte
	
	/** Data **/
	transmit[index++] = 0x2C; // , in ascii
	
	/** Checksum **/
	uint32_t bytesum = ComputeBytesum(transmit, index + 1);
	uint8_t checksum = 0xFF - (bytesum & 0xFF);
	transmit[index++] = checksum;
	
	Xbee_WriteArrayBytes(transmit, index);
}

void TransmitTest(uint8_t *packetcount, int16_t altitude)
{
	// Declare the array that will hold the bytes to transmit.
	uint8_t transmit[19]; int index = 0; int i = 0; // number needs to be changed to correct value*****
	
	transmit[index++] = 0x7E;
	transmit[index++] = 0x00;
	transmit[index++] = 0x19;
	transmit[index++] = 0x10;
	transmit[index++] = 0x01;
	transmit[index++] = 0x00;
	transmit[index++] = 0x13;
	transmit[index++] = 0xA2;
	transmit[index++] = 0x00;
	transmit[index++] = 0x40;
	transmit[index++] = 0xC0;
	transmit[index++] = 0xC5;
	transmit[index++] = 0xF1;
	transmit[index++] = 0xFF;
	transmit[index++] = 0xFE;
	transmit[index++] = 0x00;
	transmit[index++] = 0x00;
	transmit[index++] = 0x48;
	transmit[index++] = 0x65;
	transmit[index++] = 0x6C;
	transmit[index++] = 0x6C;
	transmit[index++] = 0x6F;
	transmit[index++] = 0x20;
	transmit[index++] = 0x77;
	transmit[index++] = 0x6F;
	transmit[index++] = 0x72;
	transmit[index++] = 0x6C;
	transmit[index++] = 0x64;
	transmit[index++] = 0x4A;
	
	Xbee_WriteArrayBytes(transmit, index);
}

void Save() // To be implemented*** (Erik/Maytham)
{
	
}

sample_data Sample(calibrationData bmp1caldata, AK8963_FuseRom magcaldata) // Implement multiple samples and filtering **** (requires timing analysis be done)
{
	// New parallel sampling structure 
	sample_data temp;
	// Ask for temperature from BMPs
	askTemperature1(); //askTemperature2();
	// Start the 5ms timer for BMP temperature conversion
	StartTimer(5);
	// Get the magnetometer data
	temp.MAGdata = Get_Magnetometer(magcaldata);
	// Make sure timer times out
	WaitForTimer();
	// Retrieve temperature from BMPs
	int16_t temperature1 = receiveTemperature1(); //int16_t temperature2 = receiveTemperature2();
	// Ask for pressure from BMPs
	askPressure1(); //askPressure2();
	// Start the 5ms timer for BMP pressure conversion
	StartTimer(5);
	// Get the accelerometer data
	temp.ACCdata = MPU_Get_Accel();
	// Make sure timer times out
	WaitForTimer();
	// Retrieve pressure from BMPs
	int32_t pressure1 = receivePressure1(); //int32_t pressure2 = receivePressure2();
	// Perform BMP1 calculations
	temp.BMPdata_internal = CalculateBMP(bmp1caldata, temperature1, pressure1); //temp.BMPdata_external = CalculateBMP(bmp2caldata, temperature2, pressure2);
	return temp;
	/* Previously...
	sample_data temp;
	temp.BMPdata_internal = ReadBMP1(bmp1caldata);
	temp.ACCdata = MPU_Get_Accel();
	//temp.GYRdata = MPU_Get_Gyro(); // Not necessary to save/transmit... sample only when needed
	temp.MAGdata = Get_Magnetometer(magcaldata);
	return temp;
	*/
}

/* END Scheduled functions */

/* BEGIN General functions */

int32_t CalibrateZeroAltitude(calibrationData bmp1caldata, int16_t* altitudearray, uint8_t altitudearraylength)
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
	int16_t* tofilter = calloc(filterlength, sizeof(int16_t)); // Dynamically allocate an array of size filterlength
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
	result = tofilter[filterlength/2]; // Truncates for even filterlength (avoid even filterlength value)
	free(tofilter); // Free the dynamically allocated memory
	return result;
}

AK8963_Data AverageMagData(AK8963_Data* magdatas, uint8_t numberofsamples)
{
	if (numberofsamples == 1) // If only collecting one sample retun immediately
		return magdatas[0];
	int hx = 0; int hy = 0; int hz = 0; AK8963_Data result;
	uint8_t averages = numberofsamples;
	uint8_t i;
	for (i = 0; i<numberofsamples; i++)
	{
		if(magdatas[i].Success == 0) // Only average successful samples
			averages--;
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
		return gyrodatas[0];
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

void EraseEEPROM() // To be implemented*** (Maytham)
{
	
	_eraseeeprom = false;
}

void UpdateStateEEPROM(fsw_states state) // To be implemented*** (Maytham)
{
	
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

/* END General functions */

/* BEGIN Main function */

int main(void)
{
	/* BEGIN Main variable declarations */
	
	/* BMP180 data structures */
	calibrationData bmp1caldata; // Calibration data for BMP180_1
	//calibrationData bmp2caldata; // Calibration data for BMP180_2
	int32_t initial_pressure; // Initial pressure to be read upon power up
	int16_t HistoricAltitudeData[ALTITUDE_MEMORY_LEN]; uint8_t current_altitude_pos = 0;
	/* AK8963 data structures */
	AK8963_FuseRom magcaldata; // Calibration data for the magnetometer
	/* Sensor data structure */
	sample_data HistoricSampleData[SENSOR_DATA_MEMORY_LEN]; uint8_t current_sample_pos = 0;
	/* State transition assist variables */
	uint32_t Max_Altitude = 0; // Maximum altitude seen
	/* PID motor control variables */
	int motordrive = 0; // The current drive of the motor - in the range (-100, 100)
#if MAG_STABILIZE_SAMPLES != 0
	int mag_i_term = 0; // The value of the last integral term from the magnetometer PID control system
	int mag_d_term = 0; // The value of the last derivative term from the magnetometer PID control system
#endif
#if GYRO_STABILIZE_SAMPLES != 0
	int gyro_i_term = 0; // The value of the last integral term from the gyroscope PID control system
	int gyro_d_term = 0; // The value of the last derivative term from the gyroscope PID control system
#endif
	/* Variables for transmission */
	uint8_t packetcount = 0;
	
	/* END Main variable declarations */
	
	/* BEGIN Initialization sequence */
	
	// Basic chip initialization (clock setup, variable initializations, ... )
	Basic_Init();
	
	// Initialize the GPIO
	Init_GPIO();
	
	// Initialize the MotorControl
	Init_MotorControl();
	
	// Determine start state and set initial pressure, if applicable (must be done after initializing the GPIO)
	Determine_State(&initial_pressure);
	
	// Initialize the I2C ports to be used
	I2C0_init(); //I2C1_init();
	
	// Initialize the sensors (must be done after initializing the I2C ports)
	bmp1caldata = BMP1801_init(); //bmp2caldata = BMP2_init();
	MPU_Select_ByPass(); // Set mpu6500 to bypass mode to allow reading of magnetometer
	magcaldata = GetMagnoCalibrationData(); // Get the magnetometer calibration data
	Magno_SetContinuous(); // Set the magnetometer to continuous mode
	
	// Initialize the UART ports to be used
	Xbee_Init(XBEE_BAUD); // UART0
	PC_Init(PC_BAUD); // UART2
	
	// Initialize the scheduling of periodic events (period specified at top of source; should not be changed)
	Init_Servicing(SERVICE_T);
	
	/* END Initialization sequence */
	
	/* BEGIN Testbench area */
	// THIS MUST BE COMMENTED OUT OR EMPTY FOR ANY DELIVERY
#ifdef DEBUG
	
	/* PC Communication Test
	for(;;) 
	{
		// Integer to string
		int a = 123;
		char c[20];
		sprintf(c, "%d", a);
		PC_WriteString(c);
		delay(5000);
	
		PC_WriteChar(84); //T
		PC_WriteChar(72); //H
		PC_WriteChar(73); //I
		PC_WriteChar(83); //S
		PC_WriteChar(32); // 
		PC_WriteChar(73); //I
		PC_WriteChar(83); //S
		PC_WriteChar(32); // 
		PC_WriteChar(67); //C
		PC_WriteChar(65); //A
		PC_WriteChar(78); //N
		PC_WriteChar(83); //S
		PC_WriteChar(65); //A
		PC_WriteChar(84); //T
		PC_WriteChar(32); // 
		PC_WriteChar(84); //T
		PC_WriteChar(65); //A
		PC_WriteChar(76); //L
		PC_WriteChar(75); //K
		PC_WriteChar(73); //I
		PC_WriteChar(78); //N
		PC_WriteChar(71); //G
		PC_WriteChar(13); // CR
		delay(5000);
	}*/
	
	/* Timing Tests
	uint32_t Mask_30 = 1<<30;
	
	SIM_BASE_PTR->SCGC5 |= 1<<13; // Port clock to port E
	
	PORTE_PCR30 |= 1<<8; // Set to GPIO
	
	GPIOE_PDDR |= Mask_30; // Set to output
	
	GPIOE_PCOR |= Mask_30; // Set high
	
	for(;;) 
	{
		sample_data temp;
		GPIOE_PSOR |= Mask_30;
		//temp.ACCdata = MPU_Get_Accel();
		//temp.MAGdata = Get_Magnetometer(magcaldata);
		temp = Sample(bmp1caldata, magcaldata);
		GPIOE_PCOR |= Mask_30;
		delay(200);
	}*/
	
#endif
	/* END Testbench area */
	
	/* BEGIN Main loop */
	
	for(;;)
	{
		// Perform serviced processes
		if (_transmit) // If transmit duration has passed...
		{
			TransmitTest(&packetcount, MedianFilterAltitude(HistoricAltitudeData, ALTITUDE_MEMORY_LEN, current_altitude_pos, ALTITUDE_MEDIAN_LEN));
			_transmit = false;
		}
		if (_save) // If a save duration has passed...
		{
			Save();
			_save = false;
		}
		if (_sample) // If a sample duration has passed...
		{
			// Get SampleData
			sample_data SampleData = Sample(bmp1caldata, magcaldata);
			_sample = false;
			
			// Store SampleData
			current_sample_pos++;
			if (current_sample_pos == SENSOR_DATA_MEMORY_LEN) current_sample_pos = 0;
			HistoricSampleData[current_sample_pos] = SampleData;
			
			// Compute altitude and store it
			int16_t altitude = getAltitude(SampleData.BMPdata_internal, initial_pressure);
			current_altitude_pos++;
			if (current_altitude_pos == ALTITUDE_MEMORY_LEN) current_altitude_pos = 0;
			HistoricAltitudeData[current_altitude_pos] = altitude;
			
			// Debugging prints to PC UART
#ifdef DEBUG
			/*char alt[20]; sprintf(alt, "%3d", (int)altitude); PC_WriteString("Raw:"); PC_WriteString(alt);
			
			int16_t debugaltitude = MedianFilterAltitude(HistoricAltitudeData, ALTITUDE_MEMORY_LEN, current_altitude_pos, ALTITUDE_MEDIAN_LEN);
			char print[20]; sprintf(print, "%3d", (int)debugaltitude);
			PC_WriteString(" A: "); PC_WriteString(print);
			
			char print2[20]; sprintf(print2, "%1d", (int)FSW_state);
			PC_WriteString(" FSW: "); PC_WriteString(print2);
			
			PC_WriteChar('\n');
			PC_WriteChar(13); // CR
			*/
#endif
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
					initial_pressure = CalibrateZeroAltitude(bmp1caldata, HistoricAltitudeData, ALTITUDE_MEMORY_LEN);
					// store this value in EEPROM!! ***
				}
				_statetransition = false;
				
#ifdef DEBUG2 // testing releases
				if(altitude > 3){
					// Simulated release
					bool testingrelease = true;
					for(;;)
					{
						// Begin release sequence
						// Release sequence - stage 1 (container release)
						if (testingrelease)
							{ ReleaseFromContainer(); _burnsequences = true; }
						
						testingrelease = false;
						
						// Release sequence - stage 2 (blade deployment)
						if (_releaseblades) // Counter in interrupt handler times releasess
						{
							StopReleaseFromContainer(); // Lower the container release line
							//ReleaseBlades();
							_bladesreleased = true;
						}
						_releaseblades = false; // The interrupt handler should not set _releaseblades true bc _bladesreleased is now true
						
						// Lower all release lines for safety
						if (!_burnsequences){
							//StopReleaseBlades();
						}
					}
				}
#endif				
				// Check exit condition and, if exiting, set _eraseeeprom to true
				// test jumper in place? ****
				FSW_state = LNC;
				
				break;
				
			/** LauNCh State Handling **/
			case LNC:
				// *ONLY* if exiting TST state do we erase EEPROM
				if (_eraseeeprom)
					EraseEEPROM();
				
				// If _statetransition, update state in EEPROM
				if (_statetransition)
					UpdateStateEEPROM(LNC);
				_statetransition = false;
				
				int16_t lnc_mfaltitude = MedianFilterAltitude(HistoricAltitudeData, ALTITUDE_MEMORY_LEN, current_altitude_pos, ALTITUDE_MEDIAN_LEN);
				// Keep track of max altitude seen
				if (lnc_mfaltitude > Max_Altitude)
					Max_Altitude = lnc_mfaltitude;
				
				// Check exit condition:
				if (lnc_mfaltitude > LNC_EXIT_ALTITUDE)
					{ FSW_state = ASC; _statetransition = true; }
				
				break;
				
			/** ASCent State Handling **/
			case ASC:
				// If _statetransition, update state in EEPROM
				if (_statetransition)
					UpdateStateEEPROM(ASC);
				_statetransition = false;
				
				int16_t asc_mfaltitude = MedianFilterAltitude(HistoricAltitudeData, ALTITUDE_MEMORY_LEN, current_altitude_pos, ALTITUDE_MEDIAN_LEN);
				// Keep track of max altitude seen
				if (asc_mfaltitude > Max_Altitude)
					Max_Altitude = asc_mfaltitude;
				
				// Check state exit condition:
				if (Max_Altitude - asc_mfaltitude > ASC_EXIT_BELOW_APOGEE)
				{ FSW_state = PDS; _statetransition = true; }
				
				break;
				
			/** Pre-Deployment of Scienc vehicle State Handling **/
			case PDS:
				// If _statetransition, update state in EEPROM
				if (_statetransition)
					UpdateStateEEPROM(PDS);
				_statetransition = false;
				
				// Check state exit condition:
				if (MedianFilterAltitude(HistoricAltitudeData, ALTITUDE_MEMORY_LEN, current_altitude_pos, ALTITUDE_MEDIAN_LEN) < PDS_EXIT_ALTITUDE)
					{ FSW_state = SVD; _statetransition = true; }
				
				break;
				
			/** Science Vehicle Descent State Handling **/
			case SVD:
				// If _statetransition, update state in EEPROM
				if (_statetransition)
					UpdateStateEEPROM(SVD);
				
				// Begin release sequence
				// Release sequence - stage 1 (container release)
				if (_statetransition)
					{ ReleaseFromContainer(); _burnsequences = true;} // Counter in interrupt handler times releases
				
				_statetransition = false;
				
				// Release sequence - stage 2 (blade deployment)
				if (_releaseblades)
					ReleaseBlades(); // Counter in interrupt handler times releasess
				_releaseblades = false; // Currently the interrupt handler contantly sets _releaseblades true... **** fix this
				
				// Ensure lines are low for safety
				if (!_burnsequences)
					; // ensure all lines are low ***
				
				// Extra gyroscop/magnetometer sampling and stabilisation code go here
				// Only start stabilistation after burn sequences
				if (!_burnsequences)
				{
					/* Magnetometer PID Section BEGIN */
					int magcorrection = 0;
#if MAG_STABILIZE_SAMPLES != 0
					// Average MAG_STABILIZE_SAMPLES samples from the magnetometer
					AK8963_Data magdatas[MAG_STABILIZE_SAMPLES];
					uint8_t i;
					for (i = 0; i<MAG_STABILIZE_SAMPLES; i++)
						magdatas[i] = Get_Magnetometer(magcaldata);
					AK8963_Data magaverage = AverageMagData(magdatas, MAG_STABILIZE_SAMPLES);
					
					// Calculate the error value from the magnetometer data - outputs in the range (-180, 180)
					int magerrorval = Calculate_Error_Mag(magaverage);
					
					// Calculate the correction value for the motor
					magcorrection = Calculate_Correction(magerrorval, &mag_i_term, &mag_d_term, KP_MAG, KI_MAG, KD_MAG);
#endif
					/* Magnetometer PID Section END */
					
					/* Gyroscope PID Section BEGIN */
					int gyrocorrection = 0;
#if GYRO_STABILIZE_SAMPLES != 0
					// Average GYRO_STABILIZE_SAMPLES samples from the gryoscope
					MPU6500_Gyro gyrodatas[GYRO_STABILIZE_SAMPLES];
					uint8_t j;
					for (j = 0; j<GYRO_STABILIZE_SAMPLES; j++)
							gyrodatas[i] = MPU_Get_Gyro();
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
					int setmotor = motordrive - magcorrection - gyrocorrection;
					if (setmotor == 0)
						Motor_Stop();
					else if (setmotor > 0)
						Motor_FWD(setmotor);
					else if (setmotor < 0)
						Motor_BWD(-1*setmotor); // Parameter must be a positive value
					
#ifdef DEBUG
					//PC_WriteString("Motor setpoint: ");
					//char print[20]; sprintf(print, "%4d", (int)setmotor);
#endif
					
					// Update motordrive with current value
					motordrive = setmotor;
				} //end if(!_burnsequences)
				
				// Check state exit condition:
				if (DetermineDescentDistance(HistoricAltitudeData, ALTITUDE_MEMORY_LEN, current_altitude_pos, SVD_EXIT_SAMPLES_CONSTANT) < SVD_EXIT_DEADBAND)
				{ FSW_state = LND; _statetransition = true; }
				
				break;
				
			/** LaNDed State Handling **/
			case LND:
				// If _statetransition, update state in EEPROM
				if (_statetransition)
					UpdateStateEEPROM(LND);
				
				// Activate buzzer
				if (_statetransition)
					ActivateLocationAssistanceDevice();
				
				_statetransition = false;
				
				// It is not possible to exit this state - this is the end state
				break;
				
			default:
				// Raise error and recover
				break;
		}
	}
	
	/* END Main loop */
	
}

/* END Main function */

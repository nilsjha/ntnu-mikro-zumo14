/*
borderDetect() - reads all the edge sensors and stores the sensor state in an array. 
*/
#include <Wire.h>
#include <ZumoShield.h>

#define S_BD_CLEAR 100
#define S_BD_LEFT 101
#define S_BD_RIGHT 102
#define S_BD_CENTER 103

#define BD_THRESHOLD 1500 //mi
#define NUM_SENSORS  6

int bdArrayStatus[NUM_SENSORS];
unsigned int sensor_values[NUM_SENSORS];
ZumoReflectanceSensorArray sensors(QTR_NO_EMITTER_PIN);
ZumoBuzzer buzzer;
bool DEBUG = true;					// enable debug to serial terminal
int debugdelay = 500;

int borderdirection = S_BD_CLEAR;			// start with clear border-state

void borderDetect() {
	sensors.read(sensor_values);
	for(int i = 0; i <NUM_SENSORS; i++) {
		bdArrayStatus[i] = 0;
	}

	if (sensor_values[0] < BD_THRESHOLD) {
		// XLLRRR
		bdArrayStatus[0] = 1;
	}
	if (sensor_values[5] < BD_THRESHOLD) {
		// LLLRRX
		bdArrayStatus[5] = 1;
	}
	if (sensor_values[1] < BD_THRESHOLD) {
		// LXLRRR
		bdArrayStatus[1] = 1;
	}
	if (sensor_values[4] < BD_THRESHOLD) {
		// LLLRXR
		bdArrayStatus[4] = 1;
	}
	if (sensor_values[2] < BD_THRESHOLD) {
		// LLXRRR
		bdArrayStatus[2] = 1;
	}
	if (sensor_values[3] < BD_THRESHOLD) {
		// LLLXRR
		bdArrayStatus[3] = 1;
	}

	if (bdArrayStatus[0] == 1 || bdArrayStatus[1] == 1) {
		borderdirection = S_BD_LEFT; 
	}
	else if(bdArrayStatus[4] == 0 || bdArrayStatus[5] == 1) {
		borderdirection = S_BD_RIGHT;
	}
	else if (bdArrayStatus[0] == 1 && bdArrayStatus[1] == 0 && bdArrayStatus[4] == 0 &&  bdArrayStatus[5] == 1) {
		borderdirection = S_BD_CENTER;
	}
	else if (bdArrayStatus[0] == 0 && bdArrayStatus[5] == 0) {
		borderdirection = S_BD_CLEAR;
	}
}
void borderAction() {
	switch (borderdirection)
	{
		case S_BD_LEFT:
		// what happens if the border is on the left
		break;

		case S_BD_RIGHT: 
		// what happens if the border is on the right
		break;

		case S_BD_CENTER:
		// what happens if the boarder is on both sides
		break;
		
		case S_BD_CLEAR:
		// default state and case - no boarder = coast is clear!
		break;

		default:;
	}
	if (DEBUG == true) {
		Serial.print("Borderstate: ");
		Serial.println(borderdirection);
		delay(debugdelay);
	}
}


void setup()
{
	Serial.begin(9600);

}

void loop()
{
	borderDetect();
	if (DEBUG == true) {
		for(int i = 0; i <6; i++) {
			Serial.print("Reading sensor");
			Serial.print(i);
			Serial.print(" - trigger: ");
			Serial.print(bdArrayStatus[i]);
			Serial.print(" - in value: ");
			Serial.println(sensor_values[i]);
		}
		Serial.println();
		delay(debugdelay);
	}
	borderAction();
}

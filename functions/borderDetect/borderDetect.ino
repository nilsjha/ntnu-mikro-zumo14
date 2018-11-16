#include <Wire.h>
#include <ZumoShield.h>

#define S_BD_LEFT 101
#define S_BD_RIGHT 102
#define S_BD_CENTER 103

#define BD_THRESHOLD 1500 //mi
#define NUM_SENSORS  6

int bdArrayStatus[NUM_SENSORS];
unsigned int sensor_values[NUM_SENSORS];
ZumoReflectanceSensorArray sensors(QTR_NO_EMITTER_PIN);
ZumoBuzzer buzzer;
bool DEBUG = true;

/*
borderDetect() - reads all the edge sensors and stores the sensor state in an array. 
*/
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
}
void borderAction(int borderdirection) {
	switch (borderdirection)
	{
		case S_BD_LEFT:
		break;

		case S_BD_RIGHT: 
		break;

		case S_BD_CENTER:
		break;

		default:;
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
	}
}

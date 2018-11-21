#include <Servo.h>
#include <Wire.h>
#include <ZumoShield.h>
// motors
ZumoMotors motors;

#define FORWARD_SPEED 400
#define TURN_SPEED 400
#define REVERSE_SPEED  400
#define QTR_THRESHOLD  1500


void setup() {
	pinMode(13,OUTPUT);
	Serial.begin(115200);
}

void loop () {
	Serial.println("Start");
	for (int i = 0; i < 400; i++) {
		motors.setSpeeds(i,i);
		Serial.println(i);
		digitalWrite(13,HIGH);
		delay(50);
		digitalWrite(13,LOW);
	}
}



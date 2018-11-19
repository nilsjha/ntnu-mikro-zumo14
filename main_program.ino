// all libraries
#include <Servo.h>                        //include the servo library
#include <Wire.h>
#include <ZumoShield.h>
// #include <Pushbutton.h>


//all variables
//Startbutton
int buttonPin = 12;
//Servoknockdown
Servo myservo;                            //create a servo object
int servoPosition = 90;                        //the servo will move to this position

// motors
ZumoMotors motors;

#define FORWARD_SPEED 400
#define TURN_SPEED 400
#define REVERSE_SPEED  400

unsigned long turnDuration = 5000; //TBD
unsigned long reverseDuration = 5000; //TBD

//Switch case variables
const int START_PHASE = 1;
const int ATTACK_PHASE = 2;
const int SEARCH_PHASE = 3;

Pushbutton button(ZUMO_BUTTON);

int state = START_PHASE;
unsigned long nextTimeout = 0;

// Front IR-sensor
const int FRONT_SENS_PIN = A0;			//removed trace from center bordersensor 
const double FRONT_SENS_THRESHOLD = 20; 
bool enemyDetected = false;

// Border-sensor
#define BD_THRESHOLD 1500 //mi
#define NUM_SENSORS  6

int bdArrayStatus[NUM_SENSORS];
unsigned int sensor_values[NUM_SENSORS];
ZumoReflectanceSensorArray sensors(QTR_NO_EMITTER_PIN);
const int S_BD_CLEAR = 101;
const int S_BD_LEFT = 102;
const int S_BD_RIGHT = 103;
const int S_BD_CENTER = 104;
int borderdirection = S_BD_CLEAR;

bool DEBUG = true;
const int  DEBUG_DELAY = 500;


// The function to push the ramp down before fight     
int moveRamp(int setPos)	{
	delay(2000);
	myservo.write(setPos);
	digitalWrite(13,LOW);

}

// Motors function
int runMotors(int Direction) {

	switch(Direction) {

		case 1:

			motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED); //drive forward
			break;

		case 2:

			motors.setSpeeds(TURN_SPEED, -TURN_SPEED); //swing right
			delay(turnDuration);
			break;

		case 3:
			motors.setSpeeds(-TURN_SPEED, TURN_SPEED); //swing left
			delay(turnDuration);   
			break;

		case 4:

			// if leftmost sensor detects line, reverse and turn to the right
			motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
			delay(reverseDuration);
			motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
			delay(turnDuration);
			motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);

		case 5:

			// if rightmost sensor detects line, reverse and turn to the left
			motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
			delay(reverseDuration);
			motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
			delay(turnDuration);
			motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
	}
        if(DEBUG == true) {
		Serial.print(" DEBUG: Motor-state:");
		Serial.println(Direction);
	}
}

bool frontSensor() {
        int infraValue = analogRead(FRONT_SENS_PIN);
        double infraDistance = 187754 * pow(infraValue, -1.51);

        if(DEBUG == true) {
                Serial.print(" DEBUG:FRONTSENSOR:Reading raw value: ");
                Serial.print(infraValue);
                Serial.print(" = converted value: ");
                Serial.print(infraDistance);
                Serial.print(" = threshold value: ");
                Serial.println(FRONT_SENS_THRESHOLD);
                delay(DEBUG_DELAY);
                    
        }   
        if( infraDistance < FRONT_SENS_THRESHOLD ) { 
                return true;
        } else {
                return false;
        }   

}


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
        else if(bdArrayStatus[4] == 1 || bdArrayStatus[5] == 1) {
                borderdirection = S_BD_RIGHT;
        }
        else if (bdArrayStatus[0] == 0 && bdArrayStatus[5] == 0) {
                borderdirection = S_BD_CLEAR;
        }
}




void startTimer(unsigned long timeout) {
	nextTimeout = millis() + timeout;
}
bool isTimerExpired() {
	bool statement;
	if (nextTimeout<millis()) {
		statement = true;
	} else {
		statement = false;
	}
	return statement;
}

void setup(){
        if(DEBUG == true) {
                pinMode(13,OUTPUT);
                Serial.begin(9600);
		Serial.print("Welcome! Debug mode ON, delay is ");
		Serial.println(DEBUG_DELAY);
		delay(DEBUG_DELAY);
        }   
	// startbutton
	pinMode(buttonPin, INPUT);
	//servo
	myservo.attach(6);                 //tells the servo object that it is plugged into pin 6
	myservo.write(servoPosition = 20);

}

void loop(){

	switch (state){


		case START_PHASE:
			/*startbutton
			 * Moveramp
			 * delay
			 * music    */    
			if (button.isPressed()){ 
				digitalWrite(13,HIGH);
				if (DEBUG == true) {Serial.println(" DEBUG: Button pressed!");}
				delay(5000);
				moveRamp(170);
				state = SEARCH_PHASE;
				startTimer(2000);
			}
			break;

		case ATTACK_PHASE:
			/* Search and destroy
			 *  borderdetect
			 *  runMotors();
			 *  swicth state to search phase
			 *  if true =  runmotors
			 *  if false = change state to search
			 *  
			 */
			if (DEBUG == true) {Serial.println(" DEBUG:Attack!");}
			runMotors(1);
			if(isTimerExpired()) {
				state = SEARCH_PHASE;
			}
			else {
				runMotors(1);
			}
			break;

		case SEARCH_PHASE:
			if (DEBUG == true) {Serial.println(" DEBUG:Seeking:......");}
			enemyDetected = frontSensor();
			// insert search motion here - use timer
			if(enemyDetected == true) {
				if (DEBUG == true) {Serial.println("DEBUG:ENEMY!");}
				state = ATTACK_PHASE;
				startTimer(250);
				break;
			}

			borderDetect();
			switch (borderdirection)
			{
				case S_BD_LEFT:
				// what happens if the border is on the left
				if (DEBUG == true) {Serial.println(" DEBUG: Border LEFT");}
				runMotors(4);
				break;

				case S_BD_RIGHT:
				// what happens if the border is on the right
				if (DEBUG == true) {Serial.println(" DEBUG: Border RIGHT");}
				runMotors(5);
				break;

				case S_BD_CLEAR:
				// default state and case - no boarder = coast is clear!
				break;
			}

			if (DEBUG == true) {
				Serial.print("Borderstate: ");
				Serial.println(borderdirection);
			}
			/*borderdetect
			 * sensorcheck
			 * move around (directions)
			 * if false = The robot  continues to search
			 * if true = changestate to attack
			 */

			break; 

		default:;
		if (DEBUG == true) {
			Serial.print("DEBUG: ");
			Serial.print("State is undefined!:");
		}

	}
}

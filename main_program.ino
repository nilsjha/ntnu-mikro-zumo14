// all libraries
#include <Servo.h>                        //include the servo library
#include <Wire.h>
#include <ZumoShield.h>
#include <Pushbutton.h>


//all variables
//Startbutton
int buttonPin = 12;
//Servoknockdown
Servo myservo;                            //create a servo object
int servoPosition;                        //the servo will move to this position

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
const int SEARCH_PHASE =3;

Pushbutton button(ZUMO_BUTTON);

int state = START_PHASE;

// Front IR-sensor
const int FRONT_SENS_PIN = A0; 
const double FRONT_SENS_THRESHOLD = 20; 

bool enemyDetected = false;			/*------------------------------------*/
bool DEBUG = false;				/*            DEBUG                   */
const int  DEBUG_DELAY = 500;			/*------------------------------------*/






// The function to push the ramp down before fight     
int moveRamp()	{
	delay(2000);
	myservo.write(servoPosition = 170);  
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
}

bool frontSensor() {
        int infraValue = analogRead(FRONT_SENS_PIN);
        double infraDistance = 187754 * pow(infraValue, -1.51);

        if(DEBUG == true) {
                Serial.print("Reading raw value: ");
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


void setup(){
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
				delay(5000);
				moveRamp();
				state = ATTACK_PHASE;}
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
			runMotors(1);
			break;

		case SEARCH_PHASE:
			/*borderdetect
			 * sensorcheck
			 * move around (directions)
			 * if false = The robot  continues to search
			 * if true = changestate to attack
			 */

			break; 

	}
}



/*
void setup() {
        if(DEBUG == true) {
                pinMode(13,OUTPUT);
                Serial.begin(9600);
                digitalWrite(13, LOW);
        }   


}


void loop() {
        enemyDetected = frontSensor();
        delay(100);
        if(enemyDetected == true) {
        digitalWrite(13, HIGH);
                if(DEBUG == true) {
                        Serial.print("    ENEMIES!!!     ");
                }   
        }   
        else {
                digitalWrite(13, LOW);
        }   
*/

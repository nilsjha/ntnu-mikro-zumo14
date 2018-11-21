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
#define QTR_THRESHOLD  1500

unsigned long turnDuration = 300; //TBD
unsigned long reverseDuration = 200; //TBD

//Switch case variables
const int START_PHASE = 1;
const int ATTACK_PHASE = 2;
const int SEARCH_PHASE = 3;

const int S_SEEK_LEFT = 201;
const int S_SEEK_S_LEFT = 202;
const int S_SEEK_RIGHT = 203;
const int S_SEEK_S_RIGHT = 204;
int seekState = S_SEEK_S_LEFT;


Pushbutton button(ZUMO_BUTTON);

int state = START_PHASE;
unsigned long nextTimeout = 0;
unsigned long nextSeekTimeout = 0;

// Front IR-sensor
const int FRONT_SENS_PIN = A0;                  //removed trace from center bordersensor 
const double FRONT_SENS_THRESHOLD = 30; 
bool enemyDetected = false;

// Border-sensor
#define BD_THRESHOLD 1500 //mi
#define NUM_SENSORS  6

int bdArrayStatus[NUM_SENSORS];
unsigned int sensor_values[NUM_SENSORS];
ZumoReflectanceSensorArray sensors(QTR_NO_EMITTER_PIN);

int numberOfswings = 0;

bool DEBUG = true;
const int  DEBUG_DELAY = 500;



// The function to push the ramp down before fight     
void  moveRamp(int setPos)        {
	//delay(2000);
	myservo.write(setPos);
	digitalWrite(13,LOW);

}

// Motors function
void runMotors(int Direction) {

	switch(Direction) {

		case 1:

			motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED); //drive forward
			break;

		case 2:

			motors.setSpeeds(100, 200); //swing right
			break;

		case 3:
			motors.setSpeeds(200, 100); //swing left
			break;

		case 4:

			// if leftmost sensor detects line, reverse and turn to the right
			motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
			delay(reverseDuration);
			motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
			delay(turnDuration);
			motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
			break;

		case 5:

			// if rightmost sensor detects line, reverse and turn to the left
			motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
			delay(reverseDuration);
			motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
			delay(turnDuration);
			motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
			break;
		case 6:
			motors.setSpeeds(-300,300); // STOP
			break;


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
	}   
	if( infraDistance < FRONT_SENS_THRESHOLD ) { 
		return true;
	} else {
		return false;
	}   

}
void borderDetect(){
	sensors.read(sensor_values);

	if (sensor_values[0] < QTR_THRESHOLD)
	{
		// if leftmost sensor detects line, reverse and turn to the right
		if (DEBUG == true) {Serial.println(" DEBUG: Border LEFT");}
		runMotors(4);
	}
	else if (sensor_values[5] < QTR_THRESHOLD)
	{
		// if rightmost sensor detects line, reverse and turn to the left
		if (DEBUG == true) {Serial.println(" DEBUG: Border RIGHT");}
		runMotors(5);
	}
	else
	{
		// otherwise, go straight
		// runMotors(FORWARD_SPEED, FORWARD_SPEED);
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

void startSeekTimer(unsigned long timeout) {
	nextSeekTimeout = millis() + timeout;
	if (DEBUG == true) {Serial.print(" DEBUG: SeekTimerTimeout ");}
	if (DEBUG == true) {Serial.println(nextSeekTimeout);}
}
bool isSeekTimerExpired() {
	bool statement;
	if (nextSeekTimeout<millis()) {
		statement = true;
	} else {
		statement = false;
	}
	if (DEBUG == true) {Serial.print(" DEBUG: SeekTImerexpired: ");}
	if (DEBUG == true) {Serial.println(statement);}
	return statement;
}
void seekTurn() {
	int seekIntervalLeft = 450;
	int seekIntervalRight = 450;
	int seekIntervalLeftSlow = 100;
	int seekIntervalRightSlow = 100;
	switch(seekState) {
		case S_SEEK_S_LEFT:
			if(isTimerExpired()) {
				seekState = S_SEEK_S_RIGHT;
				startTimer(seekIntervalRightSlow);
				break;
			}
			else {
				runMotors(2);
				if (DEBUG == true) {Serial.println(" DEBUG: SLOW LEFT");}
				numberOfswings++;
				break;
			}

		case S_SEEK_S_RIGHT:
			if(isTimerExpired()) {
				seekState = S_SEEK_LEFT;
				startTimer(seekIntervalLeftSlow);
				break;
			}
			else {
				runMotors(3);
				if (DEBUG == true) {Serial.println(" DEBUG: SLOW RIGHT");}
				numberOfswings++;
				break;
			}
		case S_SEEK_LEFT:
			if(isTimerExpired()) {
				seekState = S_SEEK_RIGHT;
				startTimer(seekIntervalRight);
				break;
			}
			else {
				runMotors(2);
				if (DEBUG == true) {Serial.println(" DEBUG: SEEK LEFT");}
				numberOfswings++;
				break;
			}

		case S_SEEK_RIGHT:
			if(isTimerExpired()) {
				seekState = S_SEEK_LEFT;
				startTimer(seekIntervalLeft);
				break;
			}
			else {
				runMotors(3);
				if (DEBUG == true) {Serial.println(" DEBUG: SEEK RIGHT");}
				numberOfswings++;
				break;
			}
	}
}

void seekSpin() {
	if(isSeekTimerExpired()) {
		numberOfswings = 0;
		if (DEBUG == true) {Serial.println("DEBUG: SEEK RESET!");}
	}
	else {
		runMotors(2);
		if (DEBUG == true) {Serial.println("DEBUG: SEEK TURN!");}
	}

}
void setup(){
	if(DEBUG == true) {
		pinMode(13,OUTPUT);
		Serial.begin(115200);
		Serial.print("Welcome! Debug mode ON, delay is ");
		Serial.println(DEBUG_DELAY);
		delay(DEBUG_DELAY);
	}
	// startbutton
	pinMode(buttonPin, INPUT);
	//servo
	//myservo.attach(9); 
	//myservo.write(servoPosition = 20);

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
				//moveRamp(170);
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
			//runMotors(1);
			if(isTimerExpired()) {
				state = SEARCH_PHASE;

			}
			else {
				borderDetect();
				seekState = S_SEEK_S_LEFT;
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
			//	runMotors(6);
			if (DEBUG == true) {Serial.print(" DEBUG:Number of swing:");}
			if (DEBUG == true) {Serial.println(numberOfswings);}
			if(numberOfswings == 300) {
				seekSpin();
			}
			else{
				startSeekTimer(800);
				seekTurn();
			}

			/*borderdetect
			 * sensorcheck
			 * move around (directions)
			 * if false = The robot  continues to search
			 * if true = changestate to attack
			 */

			break;

		default:
			if (DEBUG == true) {
				Serial.print("DEBUG: ");
				Serial.print("State is undefined!:");
			}

	}
}



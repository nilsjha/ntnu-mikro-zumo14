const int FRONT_SENS_PIN = A0;
const double FRONT_SENS_THRESHOLD = 20;
bool DEBUG = false;
const int  DEBUG_DELAY = 500;
bool enemyDetected = false;




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

}

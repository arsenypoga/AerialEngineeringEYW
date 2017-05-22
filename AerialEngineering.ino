#include <Adafruit_BMP183.h>
#include <Servo.h>
#include "Wire.h"

#define buttonPin 2	
#define servoPin 3	
#define ledPin 4
#define tonePin 5	//Pin for sound control
#define csPin 10	
#define echoPin 9		//Pin for sound inmput
#define triggerPin 8	//Pin for sound output

Servo cameraServo;
Adafruit_BMP183 bmp = Adafruit_BMP183(csPin);

long wait = 9000;
int initialPressure = 0;
int distance;
int msPerCm = 58;
int limitDiff;

// INFO: For every day update the sea level pressure from: http://w1.weather.gov/obhistory/KLUK.html
double seaLevelPressure = 1015.7;

void setup(void)
{
	Serial.begin(9600);
	bmp.begin();
	pinMode(servoPin, OUTPUT);
	pinMode(ledPin, OUTPUT);
	pinMode(buttonPin, INPUT_PULLUP);

	pinMode(triggerPin, OUTPUT);
	pinMode(echoPin, INPUT);

	cameraServo.attach(servoPin);
	cameraServo.write(180);

	signalRepeat(8, 300);
}

void loop()
{
	bool isOn = false;
	
	if (digitalRead(buttonPin) == LOW)
	{
		isOn = true;
		signalRepeat(3, 200);

	}

	while (isOn)
	{
		bool hasReachedHeight = false;
		
		if (bmp.getAltitude(seaLevelPressure) >= 260)
		{
			hasReachedHeight = true;
			signalRepeat(6, 300, 550);
			Serial.println("Has Reached Height");
		}



		while (hasReachedHeight)
		{
			takePhoto(300, 2000);
			
			if (averageDistance(10) <= 20)
			{
				hasReachedHeight = false;
				delay(1500);
				signalRepeat(10, 300, 1050);
				isOn = false;
				exit(0);
			}
		}


	}
		Serial.println(bmp.getAltitude(seaLevelPressure));
}

// Returns distance per given cm ratio
int getDistance()
{
	unsigned long duration;

	digitalWrite(triggerPin, LOW);
	delayMicroseconds(2);
	digitalWrite(triggerPin, HIGH);
	delayMicroseconds(10);
	digitalWrite(triggerPin, LOW);

	duration = pulseIn(echoPin, HIGH, wait);
	if (duration == 0) return 999;
	else return ((int)duration  / msPerCm);
}		   

//Return average distance per amount of times
int averageDistance(int times)
{
	for (int i = 0; i < times; i++)
	{
		int dist;
		dist =+ getDistance();
		if (i == times) {
			i = 0;
			return dist / times;
		}
	}
}

// Take picture, with given delay and cooldown
void takePhoto(int duration, int cooldown)
{
	cameraServo.write(145);
	delay(duration);
	cameraServo.write(180);
	delay(cooldown);
}

//Repeat signal given times per given duration
void signalRepeat(int times, int duration)
{
	for (int i = 0; i < times; i++)
	{
		alarm(duration);
		delay(duration);
	}
}

// Repeat signal given times, per given duration, playing given note
void signalRepeat(int times, int duration, int note)
{
	for (int i = 0; i < times; i++)
	{
		alarm(duration, note);
		delay(duration);
	}
}


void alarm(int duration)
{
	digitalWrite(ledPin, HIGH);
	tone(tonePin, 880, duration);
	delay(duration);
	digitalWrite(ledPin, LOW);
	delay(duration);

}

void alarm(int duration, int note)
{
	digitalWrite(ledPin, HIGH);
	tone(tonePin, note, duration);
	delay(duration);
	digitalWrite(ledPin, LOW);
	delay(duration);

}

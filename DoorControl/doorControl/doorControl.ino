#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"
#include <SPI.h>
#include <RTC_DS3231.h>
#include <RTClib.h>
#include <DS3231.h>

const int motorNumber = 1;     
const int forwardPin =  3;   
const int reversePin = 5;
const int alarmPin = 2;

const int maxMotorTime = 5 * 1000; //max time for motor to operate in ms
const int openDoorMotorDirection = FORWARD;
const int closeDoorMotorDirection = BACKWARD;

const int motorSpeed = 25;

#define ALRM1_MATCH_EVERY_SEC  0b1111  // once a second
#define ALRM1_MATCH_SEC        0b1110  // when seconds match
#define ALRM1_MATCH_MIN_SEC    0b1100  // when minutes and seconds match
#define ALRM1_MATCH_HR_MIN_SEC 0b1000  // when hours, minutes, and seconds match
byte ALRM1_SET = ALRM1_MATCH_SEC;

#define ALRM2_ONCE_PER_MIN     0b111   // once per minute (00 seconds of every minute)
#define ALRM2_MATCH_MIN        0b110   // when minutes match
#define ALRM2_MATCH_HR_MIN     0b100   // when hours and minutes match
byte ALRM2_SET = ALRM2_ONCE_PER_MIN;

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *doorMotor;
RTC_DS3231 Clock;

bool tempDoorState = 0;

byte year, month, date, DoW, hour, minute, second;

bool isDoorOpen()
{
  return false;
}

bool isDoorClosed()
{
  return false;
}

void openDoor()
{
  if( isDoorOpen() )
  {
    return; 
  }
  
  long startTime = millis();
  long currentTime = millis();
  long runningTime = currentTime - startTime;
    
  while( !isDoorOpen() &&
         runningTime < maxMotorTime )
  
  {
    doorMotor->run(openDoorMotorDirection);
    currentTime = millis();
    runningTime = currentTime - startTime;
  }
  
  doorMotor->run(RELEASE);  
}

void closeDoor()
{
  if( isDoorClosed() )
  {
    return; 
  }
  
  long startTime = millis();
  long currentTime = millis();
  long runningTime = currentTime - startTime;
    
  while( !isDoorClosed() &&
         runningTime < maxMotorTime )
  
  {
    doorMotor->run(closeDoorMotorDirection);
    currentTime = millis();
    runningTime = currentTime - startTime;
  }
  
  doorMotor->run(RELEASE);
}

void setup() {
  Serial.begin(9600);
  
  pinMode(forwardPin, INPUT_PULLUP);      
  pinMode(reversePin, INPUT_PULLUP);
  pinMode(alarmPin, INPUT_PULLUP);
    
  doorMotor = AFMS.getMotor(motorNumber);
  
  AFMS.begin();
  
  doorMotor->setSpeed(motorSpeed); 
  
  Wire.begin();
  Clock.begin();

  DateTime now = Clock.now();  
  

    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
}

void loop(){
  int forwardState = digitalRead(forwardPin);
  int reverseState = digitalRead(reversePin);
  
  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (forwardState == LOW) {     
    Serial.println("openDoor");
   openDoor();
  } 
  else if (reverseState == LOW) {
    Serial.println("closeDoor");
   closeDoor();
  }
  
  /*  DateTime now = RTC.now();  
  
  Serial.print("Current time is: ");
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println(); */
}

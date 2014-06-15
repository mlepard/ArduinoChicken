#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"


const int maxMotorTime = 2 * 1000; //max time for motor to operate in ms
const int openDoorMotorDirection = FORWARD;
const int closeDoorMotorDirection = BACKWARD;

const int motorSpeed = 25;

extern const int doorOpenPin;   
extern const int doorClosedPin;


Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *doorMotor;

bool isDoorOpen()
{
  return !digitalRead(doorOpenPin);;
}

bool isDoorClosed()
{
  return !digitalRead(doorClosedPin);;
}

void openDoor()
{
  if( isDoorOpen() )
  {
    Serial.println(F("Door is already open!"));    
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
    delay(100);
  }
  
  if( isDoorOpen() )
  {
    Serial.println(F("Door is now open."));
  }
  else
  {
    Serial.println(F("Motor timeout..."));
  }
  
  doorMotor->run(RELEASE);
  digitalWrite(13, HIGH);
}

void closeDoor()
{
  if( isDoorClosed() )
  {
    Serial.println(F("Door is already closed!"));
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
    delay(100);
  }
  
  if( isDoorClosed() )
  {
    Serial.println(F("Door is now closed."));
  }
  else
  {
    Serial.println(F("Motor timeout..."));
  }
  
  doorMotor->run(RELEASE);
  
  digitalWrite(13, LOW);
}

void setupDoorControl()
{
  pinMode(doorOpenPin, INPUT_PULLUP);      
  pinMode(doorClosedPin, INPUT_PULLUP);
  pinMode(13, OUTPUT);    

  doorMotor = AFMS.getMotor(motorNumber);
  
  AFMS.begin();
  
  doorMotor->setSpeed(motorSpeed);  
}


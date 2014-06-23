#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"


const int maxMotorTime = 5 * 1000; //max time for motor to operate in ms
const int openDoorMotorDirection = FORWARD;
const int closeDoorMotorDirection = BACKWARD;

const int motorSpeed = 45;

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
  
}

void setupDoorControl()
{
  initDoorControlPins();

  doorMotor = AFMS.getMotor(motorNumber);
  
  AFMS.begin();
  
  doorMotor->setSpeed(motorSpeed);  
}

void initDoorControlPins()
{
  pinMode(doorOpenPin, INPUT_PULLUP);      
  pinMode(doorClosedPin, INPUT_PULLUP);
}

void disableDoorControlPins()
{
  pinMode(doorOpenPin, OUTPUT);      
  pinMode(doorClosedPin, OUTPUT);
  digitalWrite(doorOpenPin, LOW);      
  digitalWrite(doorClosedPin, LOW);
}


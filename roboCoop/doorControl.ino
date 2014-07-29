#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"


const int maxMotorOpenTime = 8.0 * 1000; //max time for motor to operate when opening, in ms
const int maxMotorCloseTime = 11.0 * 1000; //max time for motor to operate when closing, in ms
const int openDoorMotorDirection = FORWARD;
const int closeDoorMotorDirection = BACKWARD;

const int motorOpenSpeed = 200;
const int motorCloseSpeed = 100;

extern const int doorOpenPin;   
extern const int doorClosedPin;

long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers

int doorOpenState;
int doorClosedState;
int lastButtonState = LOW;


Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *doorMotor;

bool isDoorOpen()
{
  return doorOpenState;
}

bool isDoorClosed()
{
  return doorClosedState;
}

//Shoud be called periodically, in while loop to debounce the switch..
bool debounceDoorOpen()
{
  int reading = !digitalRead(doorOpenPin);
  
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  } 
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != doorOpenState) {
      doorOpenState = reading;
    }
  }  
}

//Shoud be called periodically, in while loop to debounce the switch..
bool debounceDoorClosed()
{
  int reading = !digitalRead(doorClosedPin);
  
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  } 
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != doorClosedState) {
      doorClosedState = reading;
    }
  }  
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

  doorMotor->setSpeed(motorOpenSpeed);  
    
  while( !isDoorOpen() &&
         runningTime < maxMotorOpenTime )
  
  {
    doorMotor->run(openDoorMotorDirection);
    currentTime = millis();
    runningTime = currentTime - startTime;
    delay(10);
    debounceDoorOpen();
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
  
  doorMotor->setSpeed(motorCloseSpeed);  
    
  while( !isDoorClosed() &&
         runningTime < maxMotorCloseTime )
  
  {
    doorMotor->run(closeDoorMotorDirection);
    currentTime = millis();
    runningTime = currentTime - startTime;
    delay(10);
    debounceDoorClosed();
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
  
  doorOpenState = !digitalRead(doorOpenPin);
  doorClosedState = !digitalRead(doorClosedPin);

  doorMotor = AFMS.getMotor(motorNumber);
  
  AFMS.begin();
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


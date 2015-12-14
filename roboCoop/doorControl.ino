#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"


const int maxMotorOpenTime = 14.5 * 1000; //max time for motor to operate when opening, in ms
const int maxMotorCloseTime = 13.5 * 1000; //max time for motor to operate when closing, in ms
const int openDoorMotorDirection = FORWARD;
const int closeDoorMotorDirection = BACKWARD;

const int motorOpenSpeed = 220;
const int motorCloseSpeed = 130;

extern const int doorOpenPin;   
extern const int doorClosedPin;

long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers

int doorOpenState;
int doorClosedState;
int lastSwitchState = LOW;


Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *doorMotor;

bool isDoorOpen()
{
  return !digitalRead(doorOpenPin);
}

bool isDoorClosed()
{
  return !digitalRead(doorClosedPin);
}

//Shoud be called periodically, in while loop to debounce the switch..
void debounceDoorOpen()
{
  int reading = isDoorOpen();
  
  if( reading == LOW )
  {
    //Currently, the switch is open..we don't care about it.
    return;
  }
  else
  {      
    //The switch is now closed
    if( lastSwitchState == LOW )
    {
      //The switch was last open...therefore
      //(re)start the debounce timer running
      lastDebounceTime = millis();
    }
      
    if ((millis() - lastDebounceTime) > debounceDelay) {
      //If the reading is still high after this debounce period, the switch is permanently closed.
      doorOpenState = HIGH;
    }
  }
  lastSwitchState = reading;  
}

//Shoud be called periodically, in while loop to debounce the switch..
void debounceDoorClosed()
{
  int reading = isDoorClosed();
  
  if( reading == LOW )
  {
    return;
  }
  else
  {
    //The switch is now closed, after it was last open.
    if( lastSwitchState == LOW )
    {
      //(re)start the debounce timer running
      lastDebounceTime = millis();
    }
      
    if ((millis() - lastDebounceTime) > debounceDelay) {
      //If the reading is still high after this debounce period, the switch it permanently closed.
      doorClosedState = HIGH;
    }
  }

  lastSwitchState = reading;  
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
  
  doorOpenState = isDoorOpen();
  lastSwitchState = doorOpenState;
    
  while( !doorOpenState &&
         runningTime < maxMotorOpenTime )
  
  {
    doorMotor->run(openDoorMotorDirection);
    currentTime = millis();
    runningTime = currentTime - startTime;
    delay(5);
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

  doorClosedState = isDoorClosed();  
  lastSwitchState = doorClosedState;  
    
  while( !doorClosedState &&
         runningTime < maxMotorCloseTime )
  
  {
    doorMotor->run(closeDoorMotorDirection);
    currentTime = millis();
    runningTime = currentTime - startTime;
    delay(5);
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


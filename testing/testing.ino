#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"
#include <SPI.h>
#include <RTClib.h>
#include <RTC_DS3231.h>

const int motorNumber = 1;     
const int forwardPin =  3;   
const int reversePin = 5;

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *myMotor;

void setup() {
  Serial.begin(9600);
  
  pinMode(forwardPin, INPUT_PULLUP);      
  pinMode(reversePin, INPUT_PULLUP);
  
  myMotor = AFMS.getMotor(1);
  
  AFMS.begin();
  
  myMotor->setSpeed(50); 
  
  Wire.begin();
  RTC.begin();

  DateTime now = RTC.now();  
}

void loop(){
  int forwardState = digitalRead(forwardPin);
  int reverseState = digitalRead(reversePin);
  
  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (forwardState == LOW) {     
    //Serial.println("forward");
   myMotor->run(FORWARD); 
  } 
  else if (reverseState == LOW) {
    //Serial.println("reverse");
   myMotor->run(BACKWARD); 
  }
  else {
    //Serial.println("off");
   myMotor->run(RELEASE); 
  }
}

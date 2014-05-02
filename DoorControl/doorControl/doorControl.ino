#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"
#include <DS3231.h>
#include <avr/sleep.h>
#include <avr/power.h>

const int motorNumber = 1;     
const int forwardPin =  3;   
const int reversePin = 5;
const int alarmPin = 2;

const int maxMotorTime = 2 * 1000; //max time for motor to operate in ms
const int openDoorMotorDirection = FORWARD;
const int closeDoorMotorDirection = BACKWARD;

const int motorSpeed = 25;

#define ALRM1_MATCH_EVERY_SEC  0b1111  // once a second
#define ALRM1_MATCH_SEC        0b1110  // when seconds match
#define ALRM1_MATCH_MIN_SEC    0b1100  // when minutes and seconds match
#define ALRM1_MATCH_HR_MIN_SEC 0b1000  // when hours, minutes, and seconds match
byte ALRM1_SET = ALRM1_MATCH_HR_MIN_SEC;

#define ALRM2_ONCE_PER_MIN     0b111   // once per minute (00 seconds of every minute)
#define ALRM2_MATCH_MIN        0b110   // when minutes match
#define ALRM2_MATCH_HR_MIN     0b100   // when hours and minutes match
byte ALRM2_SET = ALRM2_MATCH_HR_MIN;

byte AlarmBits;

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *doorMotor;
DS3231 Clock;

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

//Interrupt service routine for external interrupt on INT0 pin conntected to /INT
void RTCAlarmTriggered()
{
  detachInterrupt(0);
}

void enterSleep(void)
{
  Serial.println("sleep!");
  
  
  /* Setup pin2 as an interrupt and attach handler. */
  attachInterrupt(0, RTCAlarmTriggered, LOW);
  delay(100);
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  
  sleep_enable();
  
  sleep_mode();
  
  /* The program will continue from here. */
  
  /* First thing to do is disable sleep. */
  sleep_disable(); 
}

void wakeUp()
{
  Clock.getTime(year, month, date, DoW, hour, minute, second);    
  
  Serial.print(hour, DEC);
  Serial.print(':');
  Serial.print(minute, DEC);
  Serial.print(':');
  Serial.print(second, DEC);
  Serial.println();  
}

void setup() {
  Serial.begin(9600);
  
  pinMode(forwardPin, INPUT_PULLUP);      
  pinMode(reversePin, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);      
  pinMode(13, INPUT_PULLUP);      
    
  doorMotor = AFMS.getMotor(motorNumber);
  
  Wire.begin();
  AFMS.begin();
  
  doorMotor->setSpeed(motorSpeed);
  
  Serial.println("setup!");

  Clock.getTime(year, month, date, DoW, hour, minute, second);    
  
  Serial.print(hour, DEC);
  Serial.print(':');
  Serial.print(minute, DEC);
  Serial.print(':');
  Serial.print(second, DEC);
  Serial.println();
  
  AlarmBits = ALRM2_SET;
  AlarmBits <<= 4;
  AlarmBits |= ALRM1_SET;
  
  Clock.enableOscillator(true,false, 0);


}

void loop(){
  /*int forwardState = digitalRead(forwardPin);
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
  }*/
  
  Clock.getTime(year, month, date, DoW, hour, minute, second);    
  
  
  //Clock.setA1Time(DoW, hour, minute, second + 2, AlarmBits, true, false, false);
  //Clock.setA2Time(Clock.getDate(), Clock.getHour(h12, PM), Clock.getMinute(), AlarmBits, false, false, false);
  // Turn alarms on
  //Clock.turnOnAlarm(1);
  //Clock.turnOnAlarm(2);
  
  //Clock.enableOscillator(true,false, 0);

  //enterSleep();
  //wakeUp();  
  
}

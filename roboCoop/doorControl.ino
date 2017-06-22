#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"

extern const int chickenDoorOpenPin;   
extern const int chickenDoorClosedPin;
extern const int turkeyPotAnalogPin;   

Adafruit_MotorShield AFMS = Adafruit_MotorShield();

//abstract class Sensor
class CoopDoor {
    public:
        CoopDoor();
        virtual bool isDoorOpen() =0; 
        virtual bool isDoorClosed() =0;
        virtual void openDoor();
        virtual void closeDoor();
        virtual void setupDoor();
        virtual void prepareForSleep() {};
        virtual void wakeFromSleep() {};
        
        void setupMotor( int iMotorNumber, int openDirection, int closedDireciton,
                         int maxOpenTime, int maxCloseTime,
                         int openSpeed, int closedSpeed );
 
    protected:
      int motorOpenSpeed;
      int motorCloseSpeed; 
      int motorNumber;   
      
      bool doorIsOpening;
      bool doorIsClosing;

      Adafruit_DCMotor *doorMotor;
      int openDoorMotorDirection;
      int closeDoorMotorDirection;
      int maxMotorOpenTime; //max time for motor to operate when opening, in ms
      int maxMotorCloseTime; //max time for motor to operate when closing, in ms
};

class PulleyDoor : public CoopDoor
{
  public:
    PulleyDoor();
    virtual bool isDoorOpen();
    virtual bool isDoorClosed();
    virtual void setupDoor();
    virtual void prepareForSleep();
    virtual void wakeFromSleep();
    
    void setupStatePins( int iDoorOpenPin, int iDoorClosedPin, long deboungDelay );
        
  private:    
    void initDoorControlPins();
    void disableDoorControlPins();
    bool debounceDoorClosed();
    bool debounceDoorOpen();
    void setupDoorControl();
  
    int doorOpenPin;
    int doorClosedPin;

    int lastSwitchState;
    long lastDebounceTime;  // the last time the output pin was toggled

    long debounceDelay; 
};

class LeverDoor : public CoopDoor
{
  public:
    LeverDoor();
    virtual bool isDoorOpen();
    virtual bool isDoorClosed();
    
    void setupPotSensor( int iPotSensorPint, int iDoorOpenDirection, 
                         int iDoorClosedDirection, int iSafeMaxVoltage,
                         int iSafeMinVoltage, int iDangerMinVoltage,
                         int iDangerMaxVoltage );
    
  private:
    int potSensorPin;
    float safeMinPotVoltage;
    float dangerMinPotVoltage;
    float safeMaxPotVoltage;
    float dangerMaxPotVoltage;
    int doorOpenSensorDirection;
    int doorClosedSensorDirection;
};

CoopDoor::CoopDoor()
{
  doorIsOpening = false;
  doorIsClosing = false;
}

void CoopDoor::openDoor()
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
  
  doorIsOpening = true;

  while( !isDoorOpen() &&
         runningTime < maxMotorOpenTime )
  
  {
    doorMotor->run(openDoorMotorDirection);
    currentTime = millis();
    runningTime = currentTime - startTime;
    delay(5);
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
  
  doorIsOpening = false;
}

void CoopDoor::closeDoor()
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

  doorIsClosing = true;
  
  while( !isDoorClosed() &&
         runningTime < maxMotorCloseTime )
  
  {
    doorMotor->run(closeDoorMotorDirection);
    currentTime = millis();
    runningTime = currentTime - startTime;
    delay(5);
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
  
  doorIsClosing = false;
  
}

void CoopDoor::setupMotor( int iMotorNumber, int openDirection, int closedDireciton,
                      int maxOpenTime, int maxCloseTime,
                      int openSpeed, int closedSpeed )
{
  openDoorMotorDirection = openDirection;
  closeDoorMotorDirection = closedDireciton;
  maxMotorOpenTime = maxOpenTime * 1000;
  maxMotorCloseTime = maxCloseTime * 1000;
  motorOpenSpeed = maxOpenTime;
  motorCloseSpeed = maxCloseTime;  
}
                        

PulleyDoor::PulleyDoor() :
  CoopDoor()
{
  lastSwitchState = LOW;
  lastDebounceTime = 0;
}

void PulleyDoor::setupStatePins( int iDoorOpenPin, int iDoorClosedPin, long deboungDelay )
{
  debounceDelay = deboungDelay;  
  doorOpenPin = iDoorOpenPin;
  doorClosedPin = iDoorClosedPin;
}

LeverDoor::LeverDoor() :
  CoopDoor()
{
}

void LeverDoor::setupPotSensor(int iPotSensorPin, int iDoorOpenDirection, 
                         int iDoorClosedDirection, int iSafeMaxVoltage,
                         int iSafeMinVoltage, int iDangerMinVoltage,
                         int iDangerMaxVoltage)
{
  potSensorPin = iPotSensorPin;
  doorOpenSensorDirection = iDoorOpenDirection;
  doorClosedSensorDirection = iDoorClosedDirection;
  safeMaxPotVoltage = iSafeMaxVoltage;
  safeMinPotVoltage = iSafeMinVoltage;
  dangerMaxPotVoltage = iDangerMaxVoltage;
  dangerMinPotVoltage = iDangerMinVoltage;
  
  //throw the first reading away..
  analogRead(potSensorPin);
}

bool PulleyDoor::isDoorOpen()
{
  if( !doorIsOpening )
    return !digitalRead(doorOpenPin);
  else
  {
    return debounceDoorOpen();
  }
}

bool LeverDoor::isDoorOpen()
{
  int sensorValue = analogRead(potSensorPin);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage = sensorValue * (5.0 / 1023.0);  
  
  if( voltage < dangerMinPotVoltage ||
      voltage > dangerMaxPotVoltage )
  {
    //We're over turning the Pot..stop at once!
    Serial.print(voltage);
    Serial.println(F("Pot sensor is outside the save range! STOP!"));
    return true;
  }
        
  if( doorOpenSensorDirection == HIGH )
  {     
    if( voltage >= safeMaxPotVoltage )
    {
      Serial.print(F("Open direction is high, cur: "));
      Serial.print(voltage);
      Serial.print(F("safe: "));
      Serial.println(safeMaxPotVoltage);      
      return true;
    }
    else
      return false;
  }
  else
  {    
    if( voltage <= safeMinPotVoltage )
    {
      Serial.print(F("Open direction is low, cur: "));
      Serial.print(voltage);
      Serial.print(F("safe: "));
      Serial.println(safeMinPotVoltage);
      return true;
    }
    else
      return false;
  }

  return false;
}

bool PulleyDoor::isDoorClosed()
{
  if( !doorIsClosing )
    return !digitalRead(doorClosedPin);
  else
  {
    return debounceDoorClosed();
  }  
}

bool LeverDoor::isDoorClosed()
{
  int sensorValue = analogRead(potSensorPin);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage = sensorValue * (5.0 / 1023.0);
    
  if( voltage < dangerMinPotVoltage ||
      voltage > dangerMaxPotVoltage )
  {
    //We're over turning the Pot..stop at once!
    Serial.print(voltage);
    Serial.println(F(" Pot sensor is outside the save range! STOP!"));
    return true;
  }
        
  if( doorClosedSensorDirection == HIGH )
  {
    if( voltage >= safeMaxPotVoltage )
    {
      Serial.print(F("Closed direction is high, cur: "));
      Serial.print(voltage);
      Serial.print(F("safe: "));
      Serial.println(safeMaxPotVoltage);
      return true;
    }
    else
      return false;
  }
  else
  {
    if( voltage <= safeMinPotVoltage )
    {
      Serial.print(F("Closed direction is low, cur: "));
      Serial.print(voltage);
      Serial.print(F("safe: "));
      Serial.println(safeMinPotVoltage);      
      return true;
    }
    else
      return false;
  }

  return false;
}

//Shoud be called periodically, in while loop to debounce the switch..
bool PulleyDoor::debounceDoorOpen()
{
  int reading = !digitalRead(doorOpenPin);
  
  if( reading == LOW )
  {
    //Currently, the switch is open..we don't care about it.
    return false;
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
      return true;
    }
  }
  lastSwitchState = reading;  
  return false;
}

//Shoud be called periodically, in while loop to debounce the switch..
bool PulleyDoor::debounceDoorClosed()
{
  int reading = !digitalRead(doorClosedPin);
  
  if( reading == LOW )
  {
    return false;
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
      return true;
    }
  }

  lastSwitchState = reading;
  return false;  
}

void CoopDoor::setupDoor()
{
  doorMotor = AFMS.getMotor(motorNumber);
  
  //TODO this should be in a static class function to control Doors.
  //AFMS.begin();
}

void PulleyDoor::setupDoor()
{
  initDoorControlPins();
  
  //doorOpenState = !digitalRead(doorOpenPin);
  //doorClosedState = !digitalRead(doorClosedPin);

  CoopDoor::setupDoor();
}

void PulleyDoor::prepareForSleep()
{
  disableDoorControlPins();
}

void PulleyDoor::wakeFromSleep()
{
  initDoorControlPins();
}

void PulleyDoor::initDoorControlPins()
{
  pinMode(doorOpenPin, INPUT_PULLUP);      
  pinMode(doorClosedPin, INPUT_PULLUP);
}

void PulleyDoor::disableDoorControlPins()
{
  pinMode(doorOpenPin, OUTPUT);      
  pinMode(doorClosedPin, OUTPUT);
  digitalWrite(doorOpenPin, LOW);      
  digitalWrite(doorClosedPin, LOW);
}

PulleyDoor chickenDoor;
LeverDoor turkeyDoor;

void openDoors()
{
  if( !chickenDoor.isDoorOpen() )
    chickenDoor.openDoor();
  if( !turkeyDoor.isDoorOpen() )
    turkeyDoor.openDoor();
}

void closeDoors()
{
  if( !chickenDoor.isDoorClosed() )
    chickenDoor.closeDoor();
  if( !turkeyDoor.isDoorClosed() )
    turkeyDoor.closeDoor();
}

void prepareDoorsForSleep()
{
  chickenDoor.prepareForSleep();
  turkeyDoor.prepareForSleep();
}

void wakeDoorsFromSleep()
{
  chickenDoor.wakeFromSleep();
  turkeyDoor.wakeFromSleep();
}

void setupDoorControl()
{
  chickenDoor.setupMotor(chickenMotorNumber, FORWARD, BACKWARD, 10.5, 13.5, 200, 130 );
  chickenDoor.setupStatePins( chickenDoorOpenPin, chickenDoorClosedPin, 50 );
  chickenDoor.setupDoor();
  
  turkeyDoor.setupMotor(turkeyMotorNumber, FORWARD, BACKWARD, 5.5, 5.5, 10, 10 );
  turkeyDoor.setupPotSensor( turkeyPotAnalogPin, HIGH, LOW, 3.0, 0.1, 0.0, 4.0 );
  turkeyDoor.setupDoor();
}

bool isDoorOpen( CoopDoorType door )
{
  if( door == CHICKEN )
    return chickenDoor.isDoorOpen();
  else if( door == TURKEY )
    return turkeyDoor.isDoorOpen();
  else
    return false;
}

void openDoor( CoopDoorType door )
{
  if( door == CHICKEN )
  {
    Serial.println(F("ChickenDoor - opening")); 
    chickenDoor.openDoor();
  }
  else if( door == TURKEY )
  {
    Serial.println(F("TurkeyDoor - opening")); 
    turkeyDoor.openDoor(); 
  }
}

void closeDoor( CoopDoorType door )
{
  if( door == CHICKEN )
  {
    Serial.println(F("ChickenDoor - closing"));    
    chickenDoor.closeDoor();
  }
  else if( door == TURKEY )
  {
    Serial.println(F("TurkeyDoor - closing")); 
    turkeyDoor.closeDoor();
  }    
}

void overrideDoorControl( CoopDoorType door )
{
      if( isDoorOpen(door) )
      {
        Serial.println(F("Override: close the door!"));
        closeDoor(door);
        tempIsDoorOpen = false;
      }
      else
      {
        Serial.println(F("Override: open the door!"));
        openDoor(door);
        tempIsDoorOpen = true;
      }  
}


#include "RoboCoopDefs.h"
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

extern const int DOOR_OVERRIDE_ENUM;
extern const int TIME_ALARM_ENUM;
extern const int WATCHDOG_TIMER_ENUM;
extern const boolean isWinter;

volatile boolean overrideDoor;
volatile boolean alarmHasGoneOff;
volatile boolean watchDogTimer;

extern const int alarmPin;
extern const int doorOverridePin;

void setupSleepControl()
{
  pinMode(alarmPin, INPUT_PULLUP);    
  pinMode(doorOverridePin, INPUT_PULLUP);
}

//Interrupt service routine for external interrupt on INT0 pin conntected to /INT
void RTCAlarmTriggered()
{
   alarmHasGoneOff = true;
   detachInterrupt(0);
}

//Interrupt service routine for external interrupt on INT0 pin conntected to /INT
void OverrideDoorTriggered()
{
    overrideDoor = true;  
}

WakeUpReason goToSleep(void)
{
  watchDogTimer = false;
  alarmHasGoneOff = false;
  overrideDoor = false;
  byte spi_save = SPCR;
  byte adcspa_save = ADCSRA;
  boolean pinsDisabled = false;
 
  if( !digitalRead(alarmPin) )
  {
   Serial.println(F("Can't sleep...alarm pin is active!"));
   Serial.flush();
   return  ALARM_WAKEUP;    
  } 
   

  if( isWinter || !digitalRead(doorOverridePin) )
  {
    Serial.println(F("Going to sleep via Watchdog timer..."));
    //if either of our interrupt sources are low at the moment...like
    //the switch is held down...we can't sleep properly.
    //Or it's winter...use the watchdog timer to sleep ~8s
    Serial.flush();
    //wdt_enable(WDTO_8S);
    
    attachInterrupt(0, RTCAlarmTriggered, LOW);
    if( digitalRead(doorOverridePin) )
    {
      attachInterrupt(1, OverrideDoorTriggered, LOW);
    }
    delay(100);
    
    watchdogOn();
    sleep_enable(); // Enable sleep mode.
    set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Set sleep mode.
  }
  else
  {
    Serial.println(F("Going to sleep via interrupt pins..."));
    /* Setup pin2 as an interrupt and attach handler. */
    attachInterrupt(0, RTCAlarmTriggered, LOW);
    attachInterrupt(1, OverrideDoorTriggered, LOW);
    delay(100);
  
    sleep_enable();  
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  
    spi_save = SPCR;
    SPCR = 0;
    
    adcspa_save = ADCSRA;
    ADCSRA = 0;
  
    //digitalWrite(13, LOW);
    power_adc_disable();
    power_spi_disable();
    power_timer0_disable();
    power_timer1_disable();
    power_timer2_disable();
    power_twi_disable();
    
    pinsDisabled = true;
    disableDoorControlPins();
    disableTempControlPins();
    
    MCUCR = _BV (BODS) | _BV (BODSE);  // turn on brown-out enable select
    MCUCR = _BV (BODS);        // this must be done within 4 clock cycles of above
  }      
  sleep_mode();
  
  /* The program will continue from here. */
  
  /* First thing to do is disable sleep. */
  sleep_disable();
  
 wdt_disable();  
  
 if( alarmHasGoneOff )
 {
   power_all_enable();
   SPCR = spi_save;
   ADCSRA = adcspa_save;
   
   alarmHasGoneOff = false;
   if( pinsDisabled )
   {
     initDoorControlPins();
     initTempControlPins();
   }
   Serial.println(F("Woke up via Alarm!"));
   Serial.flush();
   return  ALARM_WAKEUP;
 }
 else if( overrideDoor )
 {  
   power_all_enable();
   SPCR = spi_save;
   ADCSRA = adcspa_save;   
   overrideDoor = false;
   detachInterrupt(1);
   if( pinsDisabled )
   {
     initDoorControlPins();
     initTempControlPins();
   }
   Serial.println(F("Woke up via override door!"));
   Serial.flush();
   
   return DOOR_OVERRIDE_WAKEUP;
 }
 else if( watchDogTimer )
 {
   wdt_disable();
   watchDogTimer = false;
   Serial.println(F("Woke up via watchdog timer!"));
   Serial.flush();
   return WATCHDOG_TIMER_WAKEUP;
 }
 
 return UNKNOWN_WAKEUP;

}

void watchdogOn() {
  
// Clear the reset flag, the WDRF bit (bit 3) of MCUSR.
MCUSR = MCUSR & B11110111;
  
// Set the WDCE bit (bit 4) and the WDE bit (bit 3) 
// of WDTCSR. The WDCE bit must be set in order to 
// change WDE or the watchdog prescalers. Setting the 
// WDCE bit will allow updtaes to the prescalers and 
// WDE for 4 clock cycles then it will be reset by 
// hardware.
WDTCSR = WDTCSR | B00011000; 

// Set the watchdog timeout prescaler value to 1024 K 
// which will yeild a time-out interval of about 8.0 s.
WDTCSR = B00100001;

// Enable the watchdog timer interupt.
WDTCSR = WDTCSR | B01000000;
MCUSR = MCUSR & B11110111;

}

ISR(WDT_vect)
{
  watchDogTimer = true;
}





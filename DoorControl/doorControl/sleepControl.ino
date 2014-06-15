
extern volatile boolean overrideDoor;
extern volatile boolean alarmHasGoneOff;

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
    detachInterrupt(1);
}

void enterSleep(void)
{
  if( digitalRead(doorOverridePin) || digitalRead(alarmPin) )
  {
    //if either of our interrupt sources are low at the moment...like
    //the switch is held down...we can't sleep properly.
    return;
  }
  
    /* Setup pin2 as an interrupt and attach handler. */
  attachInterrupt(0, RTCAlarmTriggered, LOW);
  attachInterrupt(1, OverrideDoorTriggered, LOW);
  delay(100);
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  
  sleep_enable();
  
  byte spi_save = SPCR;
  SPCR = 0;
  
  //digitalWrite(13, LOW);
  power_adc_disable();
  power_spi_disable();
  power_timer0_disable();
  power_timer1_disable();
  power_timer2_disable();
  power_twi_disable();   
  
  sleep_mode();
  
  /* The program will continue from here. */
  
  /* First thing to do is disable sleep. */
  sleep_disable();
  
  power_all_enable();
 
  SPCR = spi_save; 
}

void wakeUp()
{
  if( alarmHasGoneOff )
  {
    turnOffAlarm();
    alarmHasGoneOff = false;
  }
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





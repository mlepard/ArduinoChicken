// This library contains functions to set various low-power 
// states for the ATmega328
#include <avr/sleep.h>

#include <OneWire.h> 

// This variable is made volatile because it is changed inside
// an interrupt function
volatile int sleep_count = 0; // Keep track of how many sleep
// cycles have been completed.
const int interval = 10; // Interval in minutes between waking
// and doing tasks.
const int sleep_total = (interval*60)/8; // Approximate number 
// of sleep cycles needed before the interval defined above 
// elapses. Not that this does integer math.

int DS18S20_Pin = 2; //DS18S20 Signal pin on digital 2
int PowerSwitch_Pin = 11; //Powerswitch Output pin on digital 11
float Desired_Temp = -7.0; //temperature to maintain at

//Temperature chip i/o
OneWire ds(DS18S20_Pin); // on digital pin 2

void setup(void) {
watchdogOn(); // Turn on the watch dog timer.

Serial.begin(9600);

}

void loop(void) 
{
  goToSleep(); // ATmega328 goes to sleep for about 8 seconds
  // and continues to execute code when it wakes up
  
  if (sleep_count == sleep_total) 
  {
    float temperature = getTemp();
    Serial.println(temperature);
    if( temperature < Desired_Temp )
    {
      Serial.println("Turn On");
      digitalWrite(PowerSwitch_Pin, HIGH);
    }
    else
    {
      Serial.println("Turn Off);
      digitalWrite(PowerSwitch_Pin, LOW);
    }
    Serial.flush();
    sleep_count = 0;
  }
}

float getTemp(){
 //returns the temperature from one DS18S20 in DEG Celsius

 byte data[12];
 byte addr[8];

 if ( !ds.search(addr)) {
   //no more sensors on chain, reset search
   ds.reset_search();
   return -1000;
 }

 if ( OneWire::crc8( addr, 7) != addr[7]) {
   Serial.println("CRC is not valid!");
   return -1000;
 }

 if ( addr[0] != 0x10 && addr[0] != 0x28) {
   Serial.print("Device is not recognized");
   return -1000;
 }

 ds.reset();
 ds.select(addr);
 ds.write(0x44,1); // start conversion, with parasite power on at the end

 byte present = ds.reset();
 ds.select(addr);  
 ds.write(0xBE); // Read Scratchpad

 
 for (int i = 0; i < 9; i++) { // we need 9 bytes
  data[i] = ds.read();
 }
 
 ds.reset_search();
 
 byte MSB = data[1];
 byte LSB = data[0];

 float tempRead = ((MSB << 8) | LSB); //using two's compliment
 float TemperatureSum = tempRead / 16;
 
 return TemperatureSum;
 
}

void goToSleep()   
{
// The ATmega328 has five different sleep states.
// See the ATmega 328 datasheet for more information.
// SLEEP_MODE_IDLE -the least power savings 
// SLEEP_MODE_ADC
// SLEEP_MODE_PWR_SAVE
// SLEEP_MODE_STANDBY
// SLEEP_MODE_PWR_DOWN -the most power savings
// I am using the deepest sleep mode from which a
// watchdog timer interrupt can wake the ATMega328

set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Set sleep mode.
sleep_enable(); // Enable sleep mode.
sleep_mode(); // Enter sleep mode.
// After waking from watchdog interrupt the code continues
// to execute from this point.

sleep_disable(); // Disable sleep mode after waking.
                     
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
sleep_count ++; // keep track of how many sleep cycles
// have been completed.
}


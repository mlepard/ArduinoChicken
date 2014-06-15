#include <OneWire.h> 

extern const int DS18S20_Pin; //DS18S20 Signal pin
extern const int PowerSwitch_Pin; //Powerswitch Output pin
extern const int TempLED_Pin; //Temperate LED pin on digital

float Heat_Temp = -2.0; //temperature to heat until
float Cool_Temp = -7.0; //temerpature to cool until
float Error_Temp = -1000; //Temperature returned if there is an error.
boolean isOn = false;

//Temperature chip i/o
OneWire ds(DS18S20_Pin);

void setupTempControl()
{
  pinMode(PowerSwitch_Pin, OUTPUT);
  pinMode(TempLED_Pin, OUTPUT);

  float temperature = getTemp();
  Serial.print(F("Temp Control Starting Up...temp is "));
  Serial.println(temperature);
  processTemperature(temperature);  
}

void loopTempControl()
{
  float temperature = getTemp();
  processTemperature(temperature);
}

void processTemperature( float temp )
{
  if( temp == Error_Temp )
  {
    //Ack, signal an error.
    Serial.println("Error from Temp Sensor");
    flashLED(TempLED_Pin, 5);
    //Just to be safe, turn the heat on.
    Serial.println("Turn On");
    digitalWrite(PowerSwitch_Pin, HIGH);
    return;
  }
  
  Serial.println(temp);

  if( temp <= Cool_Temp ||
      (temp < Heat_Temp && isOn)  )
  {
    //coop is below the minimum temperature OR
    //coop is between the minimum temperature and maximum
    //temperature and is heating up.
    Serial.println("Turn On");
    digitalWrite(PowerSwitch_Pin, HIGH);
    isOn = true;
    flashLED(TempLED_Pin, 2);
    delay(200);
    flashLEDTemp(TempLED_Pin, temp);  
  }
  else if( temp >= Heat_Temp ||
           (temp <= Heat_Temp && !isOn) )
  {
    //coop is above the maximum temperature OR
    //coop is between the minimum temperature and maximum
    //temperature and is cooling off.
    Serial.println("Turn Off");
    digitalWrite(PowerSwitch_Pin, LOW);
    isOn = false;
    flashLED(TempLED_Pin, 1);
    delay(200);
    flashLEDTemp(TempLED_Pin, temp);  
  }
}

void flashLED( int pin, int times )
{
  for( int ii=0; ii<times; ii++ )
  {
    digitalWrite(pin, HIGH);
    delay(200);
    digitalWrite(pin, LOW);
    delay(300);    
  }
}

void flashLEDTemp( int pin, float temperature )
{
  //if the temp is > 0, do a long flash
  if( temperature > 0.0 )
  {
    digitalWrite(pin, HIGH);
    delay(500);
    digitalWrite(pin, LOW);
    delay(200);
    return;	
  }
  
  int numTimesFlash = (int)(-1.0*temperature / 2.0) + 1;
  flashLED( pin, numTimesFlash );
}

float getTemp(){
 //returns the temperature from one DS18S20 in DEG Celsius

 byte data[12];
 byte addr[8];

 if ( !ds.search(addr)) {
   //no more sensors on chain, reset search
   ds.reset_search();
   return Error_Temp;
 }

 if ( OneWire::crc8( addr, 7) != addr[7]) {
   Serial.println("CRC is not valid!");
   return Error_Temp;
 }

 if ( addr[0] != 0x10 && addr[0] != 0x28) {
   Serial.print("Device is not recognized");
   return Error_Temp;
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


/********************************************************************/
// First we include the libraries
#include <OneWire.h> 
#include <DallasTemperature.h>
#include <Arduino.h>
#include <TM1637Display.h>

/********************************************************************/
// Data wire is plugged into pin 2 on the Arduino 
#define ONE_WIRE_BUS 4
#define CLK 2
#define DIO 3
#define ON_RELAY_TEMP 25
#define PUMP_1 5 //SOBA
#define PUMP_2 6 //CAZAN
/********************************************************************/
// Setup a oneWire instance to communicate with any OneWire devices  
// (not just Maxim/Dallas temperature ICs) 
OneWire oneWire(ONE_WIRE_BUS); 
/********************************************************************/
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
TM1637Display display(CLK, DIO);
/********************************************************************/ 
void setup(void) 
{ 
   // start serial port 
   Serial.begin(9600); 
   Serial.println("Dallas Temperature IC Control Library Demo"); 
   // Start up the library 
   sensors.begin(); 
   display.setBrightness(0x0f);
   pinMode(PUMP_1, OUTPUT);
   pinMode(PUMP_2, OUTPUT);
   onOff(true);
} 

void loop(void) 
{ 
   // call sensors.requestTemperatures() to issue a global temperature 
   // request to all devices on the bus 
  /********************************************************************/
   Serial.print(" Requesting temperatures..."); 
   sensors.requestTemperatures(); // Send the command to get temperature readings 
   Serial.println("DONE"); 
  /********************************************************************/
   Serial.print("Temperature is: "); 
   float t = sensors.getTempCByIndex(0);
   Serial.print(t); // Why "byIndex"?  
   display.showNumberDecEx(t*100, 0b11100000, false, 4, 0);
     // You can have more than one DS18B20 on the same bus.  
     // 0 refers to the first IC on the wire 
   onOff(t >= ON_RELAY_TEMP);
    
   delay(1000); 
} 
void onOff(bool onOff)
{
  if(onOff)
  {
     digitalWrite(PUMP_1, LOW);
     digitalWrite(PUMP_2, HIGH);
  }
  else
  {
     digitalWrite(PUMP_1, HIGH);
     digitalWrite(PUMP_2, LOW);
  }
}

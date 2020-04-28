/********************************************************************/
// First we include the libraries
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Arduino.h>
#include <TM1637Display.h>
#include <EEPROM.h>

/********************************************************************/
// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 4 //TEMPERATURE SENSOR
#define CLK 2 //DISPLAY
#define DIO 3 //DISPLAY
#define PUMP_1 5 //CAZAN
#define PUMP_2 6 //SOBA
#define ERR_LED 13 //ERROR
#define DEFAULT_MIN_TEMP 25
#define NEST_PIN 7

/********************************************************************/
// Setup a oneWire instance to communicate with any OneWire devices
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
/********************************************************************/
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
TM1637Display display(CLK, DIO);

/********************************************************************/
int eeAddress = 0;//EPROM ADDRESS WHERE WE STORE THE TEMPERATURE
int ON_RELAY_TEMP = DEFAULT_MIN_TEMP;
int incomingByte = 0;

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
  pinMode(ERR_LED, OUTPUT);
  pinMode(NEST_PIN, INPUT);
  EEPROM.get(eeAddress, incomingByte);
  if(incomingByte >= DEFAULT_MIN_TEMP && ON_RELAY_TEMP != incomingByte)
  {
    ON_RELAY_TEMP = incomingByte;
    Serial.print("EEPROM temp is:" );
    Serial.println(ON_RELAY_TEMP);
  }
}

bool iserr = false;

void loop(void)
{
  //READ NEW CONFIGURATION 
  Serial.print("Up Level is: ");
  Serial.println(ON_RELAY_TEMP, DEC);
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.parseInt();

    // say what you got:
    Serial.print("I received: ");
    Serial.println(incomingByte, DEC);
    if(incomingByte >= DEFAULT_MIN_TEMP && ON_RELAY_TEMP != incomingByte)
    {
        ON_RELAY_TEMP = incomingByte;
         //Store
        EEPROM.put(eeAddress, ON_RELAY_TEMP);

        incomingByte = 0;
    }
  } else {
    Serial.println("Serial not available");
  }
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  /********************************************************************/
  Serial.print(" Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperature readings
  Serial.println("DONE");
  /********************************************************************/
  Serial.print("Temperature is: ");
  float t = sensors.getTempCByIndex(0);
  Serial.println(t); // Why "byIndex"?
  if (t == -127)
  {
    if(!iserr)
    {
        Serial.println("ERROR: Cannot read temp");
        digitalWrite(ERR_LED, HIGH);
        onOff(false);
        iserr = true;
        display.clear();
    }
  }
  else
  {
    iserr = false;
    digitalWrite(ERR_LED, LOW);
    display.showNumberDecEx(t * 100, 0b11100000, false, 4, 0);
   
    onOff(t >= ON_RELAY_TEMP);
  }

  delay(1000);
}
void onOff(bool onOff)
{
  //#define PUMP_1 5 //CAZAN
  //#define PUMP_2 6 //SOBA
  if (onOff)
  {
    digitalWrite(PUMP_1, LOW);
    digitalWrite(PUMP_2, HIGH);
  }
  else
  {
    int nestStatus = digitalRead(NEST_PIN);
    Serial.print("Nest staus is: ");
    Serial.println(nestStatus);
    if(nestStatus == LOW){
      digitalWrite(PUMP_1, HIGH);
    }
    digitalWrite(PUMP_2, LOW);
  }
}

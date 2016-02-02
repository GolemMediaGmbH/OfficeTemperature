#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <Ethernet.h>

// One Wire pin for temperatur sensor
#define ONE_WIRE_BUS 2
#define LED3

// message related
#define PERIOD 30l // message period in minutes
#define SERVER "www.golem.de"
#define PATH "/projekte/ot/temp.php?"

// adjust to your setting
// see http://www.golem.de/projekte/ot/tech.php
#define TOKEN "<YOUR_TOKEN>" 
#define DBG "1"
#define PARAMS "&type=ard" // additional parameter


// Init one wire and
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// 
DeviceAddress thermometer;
EthernetClient client;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

byte errormode = 0;

void setup(void)
{
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
    
  initEthernet();

  // thermometer setup
  sensors.begin();
  sensors.getAddress(thermometer, 0);   
  sensors.setResolution(thermometer, 12);

 }

void initEthernet() {
  digitalWrite(LED, HIGH);
  if(0 == Ethernet.begin(mac)) {
    errormode = 1;
    Serial.println("no ether");
  };
  
  delay(1000);
  digitalWrite(LED, LOW);  
}

void sendTemperature(DeviceAddress deviceAddress)
{
  client.stop();
  
  float tempC = sensors.getTempC(deviceAddress); 
  if(1 == client.connect(SERVER, 80)) {
    errormode = 0;
    String cmd = "GET ";
    cmd += PATH;
    cmd += "token=";
    cmd += TOKEN;
    cmd += "&dbg=";
    cmd += DBG;
    cmd += "&temp=";
    cmd += tempC;
    cmd += PARAMS;
    cmd += " HTTP/1.0";

    client.println(cmd);
    client.print("Host: ");
    client.println(SERVER);
    client.println("Connection: close");
    client.println();
  } else {
    errormode = 1;
  }
}

void loop(void)
{ 

  static unsigned long errtime = 0;
  static unsigned long sensortime = 0;

  if(errtime < millis()) {
    errtime = millis() + 20l * 1000l;

    if(1 == errormode) {
        for(byte i = 0; i < 4; i++) {
          digitalWrite(LED, HIGH);
          delay(1000);
          digitalWrite(LED, LOW);
          delay(1000);
        }
    }
  }

  if(sensortime < millis()) {
    
    sensortime = millis() + PERIOD * 60l * 1000l;
    sensors.requestTemperatures();  
    sendTemperature(thermometer); 
    
  }
}



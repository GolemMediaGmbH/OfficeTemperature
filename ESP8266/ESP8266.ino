#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>


// Die URL sollte über das Formular erzeugt werden:
// http://www.golem.de/projekte/ot/doku.php#urlgen
// * Der Servername darf nicht enthalten sein
// * "Temperatur anhängen" auswählen
#define URL ""

// Wlan-Daten: Name des Netzwerks (SSID) und Passwort
#define SSID ""
#define PASSWORT ""

// Zeitperiode, alle 30 Minuten
#define PERIOD 30l

// Pin für den Thermometer-Sensor
#define ONE_WIRE_BUS 2

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress thermometer;

WiFiClient client;

void setup() {
  
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORT);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);    
  }    

  sensors.begin();
  sensors.getAddress(thermometer, 0); 
  sensors.setResolution(thermometer, 12);

}

void sendTemperatur(DeviceAddress deviceAddress)
{
 
  float tempC = sensors.getTempC(deviceAddress); 

  if (client.connect("www.golem.de", 80)) {
    String cmd = "GET ";
    cmd += URL;
    cmd += tempC;
    cmd += " HTTP/1.0";

    client.println(cmd);
    client.println("Host: www.golem.de");
    client.println("Connection: close");
    client.println();
    
    delay(2000);
    
    client.stop();
    
  }
}

void loop() {
  
  static unsigned long sensortime = 0;

  if(sensortime < millis()) {
    sensortime = millis() + PERIOD * 60l * 1000l;
    
    sensors.requestTemperatures(); 
    sendTemperatur(thermometer);
  }

}

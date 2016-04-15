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

// Temperaturfuehler initieren
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress thermometer;

void setup(void)
{
  // Steuert den ESP8266
  // aeltere Modelle arbeiten eventuell nur mit 9600
  Serial.begin(115200);

  // finde Temperatursensor
  sensors.begin();
  sensors.getAddress(thermometer, 0);   
  sensors.setResolution(thermometer, 12);
 }

void initEsp() {
  
  espCommand("AT+RST");  
  delay(1000);  
  espCommand("AT+CWMODE=1");
   delay(1000); 
  espCommand("AT+CIPMUX=1");
  delay(1000);  

  String cmd = "AT+CWJAP=\"";
  cmd += SSID;
  cmd += "\",\"";
  cmd += PASSWORT;
  cmd += "\"";

  Serial.println(cmd);
  Serial.find("OK\n");
  
  delay(5000);
}

void sendTemperature(DeviceAddress deviceAddress)
{
  initEsp();
  
  float tempC = sensors.getTempC(deviceAddress); 

  espCommand("AT+CIPSTART=4,\"TCP\",\"www.golem.de\",80");  
  
  delay(2000);  
  
  String cmd = "GET ";
  cmd += URL;
  cmd += tempC;
  cmd += " HTTP/1.0\r\n";
  cmd += "Host: www.golem.de\r\n\r\n";
  
  Serial.print("AT+CIPSEND=4,");
  Serial.println(cmd.length());
  
  delay(500);
  
  if(Serial.find(">")) {
    Serial.println(cmd);
    while(Serial.available()) {
      Serial.readString();  
    }
  }
  
  delay(2000);
  
  espCommand("AT+CIPCLOSE=4");
  espCommand("AT+CWQAP");
  
}

boolean espCommand(char *cmd) {
  Serial.println(cmd);
  if(Serial.find("OK\n")) {
    return true;
  }
  
  return false;
}

void loop(void)
{ 
  
  static unsigned long sensortime = 0;

  if(sensortime < millis()) {
    sensortime = millis() + PERIOD * 60l * 1000l;
    
    sensors.requestTemperatures(); 
    sendTemperature(thermometer);
  }
}



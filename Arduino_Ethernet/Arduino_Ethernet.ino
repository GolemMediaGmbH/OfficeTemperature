#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <Ethernet.h>

// Pin für den Thermometer-Sensor
#define ONE_WIRE_BUS 2

// Pin für die Info-LED
#define LED 3

// Zeitperiode, alle 30 Minuten
#define PERIOD 30l 

// Die URL sollte über das Formular erzeugt werden:
// http://www.golem.de/projekte/ot/doku.php#urlgen
// * Der Servername darf nicht enthalten sein
// * "Temperatur anhängen" auswählen
#define URL ""

#define SERVER "www.golem.de"

// Temperaturfühler initieren
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress thermometer;

// Netzwerk-Daten
EthernetClient client;

// MAC-Adresse des Ethernet-Shields, bei Bedarf eigene setzen
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

byte errormode = 0;

void setup(void)
{

  pinMode(LED, OUTPUT);
    
  initEthernet();

  // Thermometer-Sensor finden
  sensors.begin();
  sensors.getAddress(thermometer, 0);   
  sensors.setResolution(thermometer, 12);

 }

void initEthernet() {
  
  digitalWrite(LED, HIGH);
  if(0 == Ethernet.begin(mac)) {
    errormode = 1; 
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
    cmd += URL;
    cmd += tempC;
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

    if(1 == errormode) { // LED blinkt im Fehlerfall
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

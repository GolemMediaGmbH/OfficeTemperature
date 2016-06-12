/*
Für NodeMCU mit angeschlossenem Adafruit-Display SSD1306
Von Soeren Fuhrmann
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <DallasTemperature.h>
#include <Adafruit_SSD1306.h>

// Adafruit_SSD1306 OLED 128x32
// If using software SPI
#define OLED_MOSI   2 // D4
#define OLED_CLK    0 // D3
#define OLED_DC     4 // D2
#define OLED_RST    5 // D1
#define OLED_CS    16 // D0
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RST, OLED_CS);

// Die URL sollte über das Formular erzeugt werden:
// http://www.golem.de/projekte/ot/doku.php#urlgen
// * Der Servername darf nicht enthalten sein
// * "Temperatur anhaengen" auswählen
#define URL ""

// Wlan-Daten: Name des Netzwerks (SSID) und Passwort
#define SSID ""
#define PASSWORT ""

// Zeitperiode, alle 15 Minuten (maximal 20 Zeichen auf dem Display)
#define PERIOD 15
static uint8_t minutes = 1;

// Pin für den Thermometer-Sensor
#define ONE_WIRE_BUS 12 // D6
#define ONE_WIRE_3V3 13 // D7, fake psu voltage pins
#define ONE_WIRE_GND 15 // D8

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress thermometer;

WiFiClient client;

void connectWifi() {
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORT);

  display.clearDisplay();
  display.setCursor(0,16); // start for 128x32, where every other line is skipped
  display.print("conn. wifi");
  display.display();

  while (WiFi.status() != WL_CONNECTED) {
    display.print(".");
    display.display();
    delay(500);
  }

  display.clearDisplay();
  display.setCursor(0,16); // start
  display.println("wifi ip:");
  display.println(WiFi.localIP());
  display.display();

  delay(3000);
}

void setup() {
  pinMode(ONE_WIRE_3V3, OUTPUT);
  digitalWrite(ONE_WIRE_3V3, HIGH);
  pinMode(ONE_WIRE_GND, OUTPUT);
  digitalWrite(ONE_WIRE_GND, LOW);

  display.begin();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  
  connectWifi();

  sensors.begin();
  sensors.getAddress(thermometer, 0); 
  sensors.setResolution(thermometer, 12);
}

void sendTemperature(DeviceAddress deviceAddress)
{
 
  float tempC = sensors.getTempC(deviceAddress);

  minutes--;
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,16); // start

  // temp line
  display.print("temp: ");
  display.print(tempC);
  display.println("`C"); // no degree

  // countdown bar
  for (uint8_t i=0; i < minutes; i++) {
    display.write(3); // heart
  }
  display.display();

  if (minutes == 0) {
    minutes = PERIOD;
    // transmit data
    if (client.connect("www.golem.de", 80)) {
      String cmd = "GET ";
      cmd += URL;
      cmd += tempC;
      cmd += " HTTP/1.0";
  
      client.println(cmd);
      client.println("Host: www.golem.de");
      client.println("Connection: close");
      client.println();

      display.println("sent!");
      display.display();
      delay(2000);
      
      client.stop();    
    }
  }
}

void loop() {
  
  static unsigned long sensortime = 0;

  if(sensortime < millis()) {
    sensortime = millis() + 60l * 1000l;
    
    sensors.requestTemperatures(); 
    sendTemperature(thermometer);
  }

  delay(1000); // slow down loop

}


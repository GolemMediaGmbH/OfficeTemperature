#include <SPI.h>
#include <WiFi.h>

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

// Der Pin an dem der Ausgabepin des Temperatursensors TMP36
// angeschlossen ist. Einer der Analogeingänge A0 - A5
#define TMP_PIN A0

WiFiClient client;

void setup() {

  Serial.begin(9600);

  WiFi.disconnect();
  WiFi.begin(SSID, PASSWORT);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);    
  }  
  
}

void loop() {
  
  static unsigned long sensortime = 0;
  if(sensortime < millis()) {
    sensortime = millis() + PERIOD * 60l * 1000l;
  
    sendTemp(getTemp());
  }
  
}

/**
 * Ermittelt den Temperaturwert mit Hilfe eines
 * Temperatursensors TMP36
 * 
 * Der TMP36 arbeitet als Spannungsteiler
 * Der vom Sensor ermittelte Temperaturwert korrespondiert mit dem 
 * Spannungswert an dessem Ausgabepin, der am Analogeingang über ADC 
 * abgegriffen wird.
 * Es werden 10 ADC-Werte eingelesen, der Durchschnitt
 * wird wieder in die Spannung umgerechnet
 * Spannung - 0,4 * 100 = Temperatur
 * Die 0,4 wird abgezogen, da der Nullpunkt unseres TMP36 
 * bei -40 Grad liegt.
 */
float getTemp() {

  int sensorVal = 0;
  for(int i = 0, l = 10; i < l; i++) {
    sensorVal = sensorVal + analogRead(TMP_PIN);    
    delay(1);
  }
  
  float voltage = (sensorVal / 10240.0) * 5.0;
  float temperature = (voltage - .4) * 100;    

  return temperature;
}

/**
 * Wert per URL uebermitteln
 * 
 * Die Arduino-Bibliotheken für den Intel Edison sind nicht
 * 100% kompatibel zum Original. Die String-Klasse 
 * unterstuetzt keinen float. Dafuer ist die sprintf-Implementierung
 * im Gegensatz zum Original vollstaendig und unterstuetzt den
 * Datentypen.
 */
void sendTemp(float temp) {
    
  if (client.connect("www.golem.de", 80)) {

    // URL zusammenbauen
    char v[16];
    sprintf(v, "%f", temp);

    String cmd = "GET ";
    cmd += URL;
    cmd += v;
    cmd += " HTTP/1.0";

    // URL abschicken
    client.println(cmd);
    client.println("Host: www.golem.de");
    client.println("Connection: close");
    client.println();   

    client.stop();
    
  } 
}

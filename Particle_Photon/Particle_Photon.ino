#include "HttpClient/HttpClient.h"
#include "OneWire/OneWire.h"
#include "spark-dallas-temperature/spark-dallas-temperature.h"

// Pin für den Thermometer-Sensor
#define ONE_WIRE_BUS D4

// Pin für die Info-LED
#define LED D5

// Zeitperiode, alle 30 Minuten
#define PERIOD 30l // message period in minutes

// Die URL sollte über das Formular erzeugt werden:
// http://www.golem.de/projekte/ot/doku.php#urlgen
// * Der Servername muss enthalten sein
// * "Temperatur einfügen" auswählen
#define URL ""

#define SERVER "www.golem.de"

// Temperaturfühler initieren
OneWire ds(ONE_WIRE_BUS);
DallasTemperature dt(&ds);

HttpClient http;
http_request_t request;
http_response_t response;
http_header_t headers[] = {
    { NULL, NULL } 
};
   
void setup() {
   
    pinMode(LED, OUTPUT);
    request.hostname = SERVER;
    request.port = 80;

}


void loop() {
    static unsigned int nextTime = 0;
    static int errormode = 0;

    if(1 == errormode) {
        for(int i = 0; i < 4; i++) {
            digitalWrite(LED, HIGH);
            delay(500);
            digitalWrite(LED, LOW);
            delay(500);
        }
    } 

    if(nextTime < millis()) {

        dt.requestTemperatures();
        float ct = dt.getTempCByIndex(0);
        
        String path = String::format(URL, ct);
  
        request.path = path;
        request.body = "";
    
        http.get(request, response, headers);
    
        if(200 == response.status) {
            errormode = 0;
        } else {
            errormode = 1;
        }
    
        nextTime = millis() + 1000 * 60 * PERIOD;
        
    } 
}

// This #include statement was automatically added by the Particle IDE.
#include "HttpClient/HttpClient.h"

// This #include statement was automatically added by the Particle IDE.
#include "OneWire/OneWire.h"

// This #include statement was automatically added by the Particle IDE.
#include "spark-dallas-temperature/spark-dallas-temperature.h"

#define ONE_WIRE_BUS D4
#define LED D5

// message related
#define PERIOD 30l // message period in minutes
#define SERVER "www.golem.de"
#define PATH "/projekte/ot/temp.php?"

// adjust to your setting
// see http://www.golem.de/projekte/ot/tech.php
#define TOKEN "<YOUR_TOKEN>" 
#define DBG "1"
#define PARAMS "&type=wifimuc" // additional parameter

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


// Now for the loop.

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
        
        String path = String::format("%stoken=%s&temp=%f&dbg=%s%s", PATH, TOKEN, ct, DBG, PARAMS);
  
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

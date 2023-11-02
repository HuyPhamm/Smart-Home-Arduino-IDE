#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <SoftwareSerial.h>
SoftwareSerial mega(D2, D3); // TX-D2, RX-D3
// Set these to run example.
#define FIREBASE_HOST "smart-home-7f3dd-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "rMUBlph683tgx1jGDSLyR0Pspo8n3gm7lBMh9cAb"
#define WIFI_SSID "IoT LAB"
#define WIFI_PASSWORD "kvt1ptit"

void setup() {
  Serial.begin(9600);
  mega.begin(9600);

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

int n = 0;

void loop() {
  // set bool value
  while(Serial.available()){
    String data_recevied = Serial.readStringUntil('\n');
    Serial.println(data_recevied);
    float temperature, humidity, gasValue, PirValue;
    
    int scanned = sscanf(data_recevied.c_str(), "%f,%f,%f,%f", &temperature, &humidity, &gasValue, &PirValue);

    if (scanned == 4) {
      Firebase.setFloat("Sensor/temperature",temperature);
      Firebase.setFloat("Sensor/humidity",humidity);
      Firebase.setFloat("Sensor/gasValue",gasValue);
      Firebase.setFloat("Sensor/PirValue",PirValue);
    }
    bool light = Firebase.getBool("Control/light");
    bool fan = Firebase.getBool("Control/fan");
    String sent_mega = String(light) + String(fan);
    mega.println(sent_mega);
    delay(1000);
  }
}

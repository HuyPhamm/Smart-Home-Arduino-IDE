
#include <SoftwareSerial.h>
#include <SimpleTimer.h>
/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

/* Fill in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPLcPGzxQdH"
#define BLYNK_TEMPLATE_NAME "DHT22"
#define BLYNK_AUTH_TOKEN "iP-nTk48vhcwvrQ6IjAG2u7qNCJfj75d"

#include <Wire.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "VIETTEL LAB";
char pass[] = "kvt1ptit";
BlynkTimer timer;

SoftwareSerial uno(D2,D3); // TX-2, RX-3
void setup() {
  Serial.begin(9600);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(1000L, sendDataToBlynk);
}

void loop() {
  Blynk.run();
  timer.run();
}

void sendDataToBlynk() {
  while (Serial.available()) {
    String data = Serial.readStringUntil('\n');
    Serial.println(data);
    float temperature, humidity, gasValue, PirValue;
    
    // Giả sử dữ liệu được gửi từ Mega 2560 có định dạng là "temperature,humidity,gasValue,PirValue"
    int scanned = sscanf(data.c_str(), "%f,%f,%f,%f", &temperature, &humidity, &gasValue, &PirValue);

    if (scanned == 4) {
      Blynk.virtualWrite(V1, temperature);
      Blynk.virtualWrite(V0, humidity);
      // Gửi các giá trị gasValue và PirValue lên các pin Blynk tương ứng (thay thế V2 và V3 bằng các pin Blynk bạn đang sử dụng).
      Blynk.virtualWrite(V2, gasValue);
      Blynk.virtualWrite(V3, PirValue);
    } else {
      Serial.println("Invalid data format");
    }
  }
}

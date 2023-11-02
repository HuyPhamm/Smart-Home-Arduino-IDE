#include <SoftwareSerial.h>
SoftwareSerial nodemcu(2, 3); // TX-2, RX-3
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
#include <Keypad.h>
#define LED_Pin 50
#define PIROUT 47
#include <DHT.h>
#define DHTPIN 40
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

int analogPin = A15; // Pin connected to MQ-02 sensor
int buzzerPin = 53;  // Pin connected to the buzzer
int fan = 49; // Pin connected to the fan 5V

// RGB LED pin definitions
int red = 31;
int green = 33;
int blue = 35;

//
int led_data = 37;
int zig_bee_led = 41;
// Keypad definitions
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'}, {'4', '5', '6', 'B'}, {'7', '8', '9', 'C'}, {'*', '0', '#', 'D'}};
byte rowPins[ROWS] = {22, 24, 26, 28};
byte colPins[COLS] = {30, 32, 34, 36};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

char STR[4] = {'0', '0', '0', '0'}; // Set your password here
char str[4] = {' ', ' ', ' ', ' '};
int i, count = 0;
int check = 0;
int relay = 52;
int moto = 49;
int checkkk = 0;
int threshold = 400;

void setup() {
  lcd.init();
  lcd.backlight();
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(115200);
  nodemcu.begin(115200); // Khởi tạo UART cho NodeMCU
  lcd.begin(16, 2);
  pinMode(led_data,OUTPUT);
  pinMode(LED_Pin, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(relay, OUTPUT);
  pinMode(moto, OUTPUT);
  pinMode(fan,OUTPUT);
  pinMode(zig_bee_led,OUTPUT);
  delay(1000);
}
//Data Uno to NodeMCU
float temperature, humidity, gasValue, PirValue;
float trigerGas;

//Data NodeMCU to Uno
char LedData,FanStatus;

void sendDataToNodeMCU() {
  String data = String(temperature) + "," + String(humidity) + "," + String(trigerGas) + "," + String(PirValue);
  nodemcu.println(data); // Gửi dữ liệu qua UART tới Wemos D1 R1
  Serial.print("Data sent to NodeMCU: ");
  Serial.println(data); // In dữ liệu đã gửi ra Serial Monitor
}


void loop() {
  readGasSensor();
  checkPIRSensor();
  readDHTSensor();
  receviedDatatoNodeMCU();
  sendDataToNodeMCU();
  updateLCD();
  active_led_zigbee();
  active_led_data();
  delay(1000);
}

void readGasSensor() {
  gasValue = analogRead(analogPin);
  Serial.print("Gas Sensor Analog Value: ");
  Serial.println(gasValue);

  if (gasValue > threshold) {
    trigerGas = 1;
    triggerGasAlert();
  } else if (check == 1) {
    trigerGas = 0;
    resetGasAlert();
  }
}

void triggerGasAlert() {
  Serial.println("Gas alert: High gas level detected!");
  activateAlertComponents();
}

void resetGasAlert() {
  Serial.println("Gas alert: Gas level normal.");
  deactivateAlertComponents();
}

void activateAlertComponents() {
  digitalWrite(buzzerPin, HIGH);
  digitalWrite(red, HIGH);
  delay(50);
  digitalWrite(red, LOW);
  delay(50);
  digitalWrite(buzzerPin, LOW);
  digitalWrite(green, HIGH);
  delay(50);
  digitalWrite(green, LOW);
  delay(50);
  digitalWrite(buzzerPin, HIGH);
  digitalWrite(blue, HIGH);
  delay(50);
  digitalWrite(blue, LOW);
  delay(50);
  digitalWrite(buzzerPin, LOW);
  check = 1;
}

void deactivateAlertComponents() {
  digitalWrite(buzzerPin, LOW);
  check = 0;
}

char readKeypadInput() {
  char key = keypad.getKey();
  if (key) {
    handleKeypadInput(key);
    return key; // Trả về nút từ bàn phím
  }
  return ' '; // Trả về khoảng trắng nếu không có nút nào được nhấn
}

void handleKeypadInput(char key) {
  if (i < 4) {
    str[i] = key;
    i++;

    if (i == 4) {
      checkPassword();
    }
  }
}

void checkPassword() {
  if (strcmp(str, STR) == 0) {
    openLock();
  } else {
    i = 0;
  }
}

void openLock() {
  Serial.println("Correct password entered. Opening the lock.");
  digitalWrite(relay, HIGH);
  i = 0;
}

void checkPIRSensor() {
  PirValue = digitalRead(PIROUT);

  if (PirValue == 1) {
    handleMotionDetected();
  } else {
    handleNoMotionDetected();
  }
}

void handleMotionDetected() {
  if (checkkk == 1) {
    checkkk = 0;
  }
  Serial.println("Some body is here");
}

void handleNoMotionDetected() {
  if (checkkk == 0) {
    checkkk = 1;
  }
  Serial.println ("No body is here");
}

void readDHTSensor() {
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
}

void receviedDatatoNodeMCU(){
  if(Serial.available()){
    String Data = Serial.readStringUntil('\n');
    Serial.print("Data recived from NodeMCU: ");
    Serial.println(Data);
    char LedData = Data[0];
    char FanStatus = Data[2];
    //Serial.println(LedData);
    //Serial.println(FanStatus);

    // Bat Tat Den 220V
    Serial.print("Trang Thai Den: ");
    if(LedData == '1')
      digitalWrite(moto,HIGH);
    else
      digitalWrite(moto,LOW);
    Serial.println(LedData);

    // Bat Tat Quat
    Serial.print("Trang Thai Quat: ");
    if(FanStatus == 1)
      digitalWrite(fan,HIGH);
    else
      digitalWrite(fan,LOW);
  
    Serial.println(FanStatus);
  }
}

void updateLCD() {
  lcd.setCursor(0, 0);
  lcd.print("Temperature: ");
  lcd.print(temperature);
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(humidity);
  lcd.print("%");
}

void active_led_zigbee(){
  digitalWrite(zig_bee_led,HIGH);
  delay(50);
  digitalWrite(zig_bee_led,LOW);
  delay(50);
}

void active_led_data(){
  digitalWrite(led_data,HIGH);
  delay(50);
  digitalWrite(led_data,LOW);
  delay(50);
}
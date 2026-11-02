
#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <U8g2lib.h>

// =====================================
// Wi-Fi Configuration
// =====================================
const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// ESP32 #1 transmitter IP address.
// Replace with the IP printed by ESP32 #1.
const char* TRANSMITTER_IP = "192.168.1.50";

String transmitterURL =
  String("http://") + TRANSMITTER_IP;

// =====================================
// ESP32 #2 Pin Configuration
// =====================================
#define VOLTAGE_SENSOR_PIN 34
#define CURRENT_SENSOR_PIN 35

#define OLED_SDA 21
#define OLED_SCL 22

// =====================================
// OLED Configuration (SH1106)
// =====================================
U8G2_SH1106_128X64_NONAME_F_HW_I2C oled(
  U8G2_R0, U8X8_PIN_NONE
);

// =====================================
// Sensor Calibration
// =====================================
const float ADC_REFERENCE = 3.3;
const float ADC_MAX = 4095.0;

// Set these according to your actual modules
// and calibrate against a multimeter.
const float VOLTAGE_CALIBRATION = 5.0;

// This assumes the 5 A ACS712 version.
// 20 A and 30 A versions need different values.
const float ACS712_SENSITIVITY = 0.066;

float currentZeroVoltage = 2.5;

// =====================================
// Battery Configuration
// =====================================
const float BATTERY_MIN_VOLTAGE = 3.30;
const float BATTERY_MAX_VOLTAGE = 4.20;

const float CUTOFF_SOC = 100.0;

// =====================================
// Monitoring Variables
// =====================================
float batteryVoltage = 0.0;
float chargingCurrent = 0.0;
float chargingPower = 0.0;
float stateOfCharge = 0.0;

bool chargingStatus = false;
bool cutoffReached = false;

unsigned long previousMillis = 0;
const unsigned long INTERVAL = 1000;

// =====================================
// Read Average ADC Voltage
// =====================================
float readSensorVoltage(int pin) {
  long total = 0;
  const int samples = 50;

  for (int i = 0; i < samples; i++) {
    total += analogRead(pin);
    delay(2);
  }

  float averageADC = total / (float)samples;

  return (averageADC / ADC_MAX) * ADC_REFERENCE;
}

// =====================================
// Read Battery Voltage
// =====================================
float readBatteryVoltage() {
  float sensorVoltage =
    readSensorVoltage(VOLTAGE_SENSOR_PIN);

  return sensorVoltage * VOLTAGE_CALIBRATION;
}

// =====================================
// Read Charging Current
// =====================================
float readChargingCurrent() {
  float sensorVoltage =
    readSensorVoltage(CURRENT_SENSOR_PIN);

  float current =
    (sensorVoltage - currentZeroVoltage)
    / ACS712_SENSITIVITY;

  if (current < 0.0) {
    current = 0.0;
  }

  return current;
}

// =====================================
// Calculate Battery SOC
// =====================================
float calculateSOC(float voltage) {
  float soc =
    ((voltage - BATTERY_MIN_VOLTAGE) /
    (BATTERY_MAX_VOLTAGE - BATTERY_MIN_VOLTAGE))
    * 100.0;

  return constrain(soc, 0.0, 100.0);
}

// =====================================
// Send Command to ESP32 #1
// =====================================
bool sendTransmitterCommand(String command) {

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi disconnected");
    return false;
  }

  HTTPClient http;

  String url = transmitterURL + "/" + command;

  http.begin(url);
  http.setTimeout(3000);

  int responseCode = http.GET();

  Serial.print("Transmitter response: ");
  Serial.println(responseCode);

  http.end();

  return responseCode == 200;
}

// =====================================
// OLED Display
// =====================================
void displayReadings() {

  oled.clearBuffer();
  oled.setFont(u8g2_font_6x10_tf);

  oled.drawStr(0, 10, "WIRELESS EV CHARGING");

  oled.setCursor(0, 22);
  oled.print("Status: ");
  oled.print(chargingStatus ? "CHARGING" : "STOPPED");

  oled.setCursor(0, 33);
  oled.print("SOC: ");
  oled.print(stateOfCharge, 0);
  oled.print("%");

  oled.setCursor(0, 44);
  oled.print("Voltage: ");
  oled.print(batteryVoltage, 2);
  oled.print(" V");

  oled.setCursor(0, 55);
  oled.print("Current: ");
  oled.print(chargingCurrent, 2);
  oled.print(" A");

  oled.setCursor(0, 64);
  oled.print("Power: ");
  oled.print(chargingPower, 2);
  oled.print(" W");

  oled.sendBuffer();
}

// =====================================
// Serial Monitor Output
// =====================================
void printReadings() {

  Serial.println("--------------------------");

  Serial.print("Battery Voltage: ");
  Serial.print(batteryVoltage, 2);
  Serial.println(" V");

  Serial.print("Charging Current: ");
  Serial.print(chargingCurrent, 2);
  Serial.println(" A");

  Serial.print("Charging Power: ");
  Serial.print(chargingPower, 2);
  Serial.println(" W");

  Serial.print("Battery SOC: ");
  Serial.print(stateOfCharge, 1);
  Serial.println("%");

  Serial.print("Charging Status: ");
  Serial.println(chargingStatus ? "CHARGING" : "STOPPED");
}

// =====================================
// Setup
// =====================================
void setup() {

  Serial.begin(115200);

  analogReadResolution(12);

  analogSetPinAttenuation(
    VOLTAGE_SENSOR_PIN, ADC_11db
  );

  analogSetPinAttenuation(
    CURRENT_SENSOR_PIN, ADC_11db
  );

  // Initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  oled.begin();

  oled.clearBuffer();
  oled.setFont(u8g2_font_6x10_tf);
  oled.drawStr(0, 20, "EV RECEIVER");
  oled.drawStr(0, 35, "Connecting Wi-Fi...");
  oled.sendBuffer();

  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi connected");

  Serial.print("ESP32 #2 IP: ");
  Serial.println(WiFi.localIP());

  // Disable transmitter before current sensor
  // zero-offset calibration.
  sendTransmitterCommand("stop");

  delay(500);

  // Calibrate current sensor with zero current.
  currentZeroVoltage =
    readSensorVoltage(CURRENT_SENSOR_PIN);

  Serial.print("ACS712 zero offset: ");
  Serial.println(currentZeroVoltage, 3);

  Serial.println("Receiver monitoring started");
}

// =====================================
// Main Loop
// =====================================
void loop() {

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= INTERVAL) {

    previousMillis = currentMillis;

    // Read sensor values
    batteryVoltage = readBatteryVoltage();

    chargingCurrent = readChargingCurrent();

    // Calculate charging power
    chargingPower =
      batteryVoltage * chargingCurrent;

    // Calculate battery SOC
    stateOfCharge =
      calculateSOC(batteryVoltage);

    // Automatic charging control
    if (stateOfCharge >= CUTOFF_SOC &&
        !cutoffReached) {

      if (sendTransmitterCommand("stop")) {

        chargingStatus = false;
        cutoffReached = true;

        Serial.println("Battery threshold reached");
        Serial.println("Transmitter stopped");
      }
    }

    // Display and print readings
    displayReadings();
    printReadings();
  }
}

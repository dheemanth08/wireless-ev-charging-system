
#include <WiFi.h>
#include <WebServer.h>

// Wi-Fi credentials
const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// Transmitter control pin
const int MOSFET_GATE_PIN = 26;

// HTTP server
WebServer server(80);

bool chargingEnabled = false;

// Stop wireless charging
void stopCharging() {
  digitalWrite(MOSFET_GATE_PIN, LOW);
  chargingEnabled = false;

  Serial.println("Charging stopped");
  server.send(200, "text/plain", "Charging stopped");
}

// Start wireless charging
void startCharging() {
  digitalWrite(MOSFET_GATE_PIN, HIGH);
  chargingEnabled = true;

  Serial.println("Charging started");
  server.send(200, "text/plain", "Charging started");
}

// Return transmitter status
void getStatus() {
  if (chargingEnabled) {
    server.send(200, "text/plain", "CHARGING");
  } else {
    server.send(200, "text/plain", "STOPPED");
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(MOSFET_GATE_PIN, OUTPUT);

  // Keep transmitter disabled at startup
  digitalWrite(MOSFET_GATE_PIN, LOW);

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

  Serial.print("Transmitter IP Address: ");
  Serial.println(WiFi.localIP());

  // Configure HTTP endpoints
  server.on("/start", HTTP_GET, startCharging);
  server.on("/stop", HTTP_GET, stopCharging);
  server.on("/status", HTTP_GET, getStatus);

  // Start server
  server.begin();

  Serial.println("Transmitter HTTP server started");
}

void loop() {
  server.handleClient();
}

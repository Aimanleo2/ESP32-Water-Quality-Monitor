/*
 * ======================================
 * MAIN PROGRAM - ESP32 Water Quality Monitor
 * ======================================
 * Sistem Monitor Kualiti Air dengan:
 * - Sensor Turbidity (Kekeruhan) - Analog
 * - Sensor pH (E-201-C) - Analog
 * - Blynk untuk notifikasi dan dashboard
 * - WiFi connection
 */

#define BLYNK_PRINT Serial  // Enable Blynk debug
#include <BlynkSimpleEsp32.h>
#include "config.h"
#include "sensors.h"
#include "wifi_manager.h"
#include "blynk_manager.h"

// ========== Deklarasi Objek Global ==========
TurbiditySystem turbidity;
PHSystem ph;
WiFiManager wifiManager;
BlynkManager blynkManager;

// ========== Timing Variables ==========
unsigned long lastSensorReadTime = 0;
unsigned long lastBlynkSendTime = 0;

// ========== Setup Function ==========
void setup() {
  // Initialize Serial Communication
  Serial.begin(SERIAL_BAUD_RATE);
  delay(1000);
  
  Serial.println("\n\n");
  Serial.println("===========================================");
  Serial.println("  ESP32 Water Quality Monitor");
  Serial.println("  Sistem Monitor Kualiti Air");
  Serial.println("===========================================");
  Serial.println("Initializing...");
  
  // Initialize Pins
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  // Initialize Sensors
  Serial.println("[1/4] Initializing Sensors...");
  turbidity.init();
  ph.init();
  Serial.println("      ✓ Sensors initialized");
  
  // Initialize WiFi
  Serial.println("[2/4] Connecting to WiFi...");
  wifiManager.connect(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("      ✓ WiFi connected");
  
  // Initialize Blynk
  Serial.println("[3/4] Connecting to Blynk...");
  blynkManager.begin(BLYNK_AUTH_TOKEN);
  Serial.println("      ✓ Blynk connected");
  
  // Test Initial Read
  Serial.println("[4/4] Performing initial sensor read...");
  turbidity.read();
  ph.read();
  Serial.println("      ✓ Initialization Complete!");
  
  Serial.println("===========================================");
  Serial.println("System Ready! Starting monitoring...\n");
  
  // LED blink untuk menunjukkan sistem siap
  for(int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
}

// ========== Main Loop ==========
void loop() {
  // Maintain WiFi connection
  if (!wifiManager.isConnected()) {
    Serial.println("⚠ WiFi disconnected. Reconnecting...");
    wifiManager.connect(WIFI_SSID, WIFI_PASSWORD);
  }
  
  // Maintain Blynk connection
  if (Blynk.connected()) {
    Blynk.run();
  } else {
    Serial.println("⚠ Blynk disconnected. Reconnecting...");
    Blynk.connect();
  }
  
  // Read sensors at regular interval
  if (millis() - lastSensorReadTime >= SENSOR_READ_INTERVAL) {
    lastSensorReadTime = millis();
    readAllSensors();
  }
  
  // Send data to Blynk at regular interval
  if (millis() - lastBlynkSendTime >= BLYNK_SEND_INTERVAL) {
    lastBlynkSendTime = millis();
    sendDataToBlynk();
    checkThresholds();
  }
  
  delay(10);  // Small delay to prevent WDT reset
}

// ========== Function: Read All Sensors ==========
void readAllSensors() {
  Serial.println("\n--- Reading Sensors ---");
  
  // Baca sensor turbidity
  turbidity.read();
  Serial.print("Turbidity: ");
  Serial.print(turbidity.getNTU());
  Serial.println(" NTU");
  
  // Baca sensor pH
  ph.read();
  Serial.print("pH: ");
  Serial.println(ph.getPH(), 2);
  
  // Print status water quality
  printWaterQualityStatus();
}

// ========== Function: Send Data to Blynk ==========
void sendDataToBlynk() {
  if (!Blynk.connected()) return;
  
  // Send Turbidity value to Virtual Pin V0
  Blynk.virtualWrite(V0, turbidity.getNTU());
  Serial.print("Sent Turbidity to Blynk: ");
  Serial.print(turbidity.getNTU());
  Serial.println(" NTU");
  
  // Send pH value to Virtual Pin V1
  Blynk.virtualWrite(V1, ph.getPH());
  Serial.print("Sent pH to Blynk: ");
  Serial.println(ph.getPH(), 2);
  
  // Send raw ADC values for debugging
  Blynk.virtualWrite(V2, turbidity.getRawADC());  // Raw turbidity ADC
  Blynk.virtualWrite(V3, ph.getRawADC());         // Raw pH ADC
}

// ========== Function: Check Thresholds & Send Alerts ==========
void checkThresholds() {
  bool alertTriggered = false;
  String alertMessage = "";
  
  // Check Turbidity Threshold
  if (turbidity.getNTU() > TURBIDITY_THRESHOLD) {
    alertTriggered = true;
    alertMessage += "⚠ Kekeruhan Tinggi: " + String(turbidity.getNTU(), 1) + " NTU\n";
    Serial.println("🚨 ALERT: Turbidity exceeded threshold!");
  }
  
  // Check pH Minimum Threshold
  if (ph.getPH() < PH_MIN_THRESHOLD) {
    alertTriggered = true;
    alertMessage += "⚠ pH Terlalu Asid: " + String(ph.getPH(), 2) + "\n";
    Serial.println("🚨 ALERT: pH too acidic!");
  }
  
  // Check pH Maximum Threshold
  if (ph.getPH() > PH_MAX_THRESHOLD) {
    alertTriggered = true;
    alertMessage += "⚠ pH Terlalu Alkalin: " + String(ph.getPH(), 2) + "\n";
    Serial.println("🚨 ALERT: pH too alkaline!");
  }
  
  // Send alert to Blynk if triggered
  if (alertTriggered) {
    blynkManager.sendAlert(alertMessage);
    digitalWrite(LED_PIN, HIGH);  // Turn on LED as indicator
  } else {
    digitalWrite(LED_PIN, LOW);   // Turn off LED when normal
  }
}

// ========== Function: Print Water Quality Status ==========
void printWaterQualityStatus() {
  String quality = "";
  
  if (turbidity.getNTU() <= 1.0) quality = "Sangat Bersih";
  else if (turbidity.getNTU() <= 5.0) quality = "Bersih";
  else if (turbidity.getNTU() <= 10.0) quality = "Sederhana";
  else quality = "Keruh";
  
  Serial.print("Status Air: ");
  Serial.println(quality);
  
  if (ph.getPH() >= PH_MIN_THRESHOLD && ph.getPH() <= PH_MAX_THRESHOLD) {
    Serial.println("Status pH: ✓ Normal");
  } else if (ph.getPH() < PH_MIN_THRESHOLD) {
    Serial.println("Status pH: ✗ Terlalu Asid");
  } else {
    Serial.println("Status pH: ✗ Terlalu Alkalin");
  }
}

// ========== Blynk Virtual Pin Handlers ==========
// Virtual Pin V4 - Button untuk manual refresh
BLYNK_WRITE(V4) {
  if (param.asInt()) {
    Serial.println("Manual refresh triggered from app");
    readAllSensors();
    sendDataToBlynk();
  }
}

// Virtual Pin V5 - Display connection status
BLYNK_WRITE(V5) {
  // Ini adalah read-only, diupdate dari loop
}

BLYNK_CONNECTED() {
  Serial.println("✓ Blynk app connected!");
  // Sync all virtual pins
  Blynk.syncVirtual(V4, V5);
}

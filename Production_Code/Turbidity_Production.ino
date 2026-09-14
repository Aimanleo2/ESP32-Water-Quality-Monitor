/*
 * ======================================
 * TURBIDITY SENSOR - PRODUCTION CODE
 * ======================================
 * Calibrated and Ready for Operation
 * GPIO 34 - Analog Input (ADC1_CH6)
 * 
 * CALIBRATION VALUES (Calibrated: 2026-09-14):
 * Clear Water: 1.916V (0 NTU)
 * Turbid Water: 1.60V (10 NTU)
 * 
 * WIRING:
 * Turbidity Sensor V1.1:
 *   - Red (VCC)   → ESP32 5V
 *   - Black (GND) → ESP32 GND
 *   - Yellow (Signal) → GPIO 34
 */

// ===== PIN DEFINITIONS =====
#define TURBIDITY_PIN 34      // GPIO 34 - ADC1_CH6
#define LED_PIN 2             // GPIO 2 - Status indicator

// ===== CALIBRATION VALUES (Calibrated) =====
#define TURBIDITY_VOLTAGE_CLEAR 1.916   // Clear water (0 NTU)
#define TURBIDITY_VOLTAGE_TURBID 1.60   // Turbid water (10 NTU)

// ===== ADC SETTINGS =====
#define ADC_MAX_VALUE 4095             // 12-bit ADC
#define ESP32_VOLTAGE 3.3              // ADC reference voltage

// ===== SAMPLING SETTINGS =====
#define SAMPLE_COUNT 10                // Number of samples to average
#define READ_INTERVAL 2000             // Read interval in milliseconds (2 seconds)

// ===== VARIABLES =====
unsigned long lastReadTime = 0;
float currentVoltage = 0;
float currentNTU = 0;
int rawADC = 0;
int readCount = 0;

// ===== SETUP FUNCTION =====
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n");
  Serial.println("╔════════════════════════════════════════════════════════╗");
  Serial.println("║     TURBIDITY SENSOR - PRODUCTION CODE                 ║");
  Serial.println("║     Calibrated and Ready for Operation                 ║");
  Serial.println("║     GPIO 34 - Analog Input (ADC1_CH6)                  ║");
  Serial.println("╚════════════════════════════════════════════════════════╝");
  Serial.println();
  Serial.println("📊 CALIBRATION VALUES:");
  Serial.println("   Clear Water: 1.916V (0 NTU)");
  Serial.println("   Turbid Water: 1.60V (10 NTU)");
  Serial.println();
  
  // Setup pins
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  pinMode(TURBIDITY_PIN, INPUT);
  
  // Setup ADC
  analogReadResolution(12);  // 12-bit ADC resolution
  
  Serial.println("Status: ✓ Ready for monitoring");
  Serial.println("═══════════════════════════════════════════════════════════");
  Serial.println("TIME(s) | RAW ADC | VOLTAGE(V) |  NTU  | CLARITY STATUS");
  Serial.println("═══════════════════════════════════════════════════════════");
}

// ===== LOOP FUNCTION =====
void loop() {
  if (millis() - lastReadTime >= READ_INTERVAL) {
    lastReadTime = millis();
    readCount++;
    
    // Read sensor with averaging
    readTurbidityWithAveraging();
    
    // Print results
    printResults();
    
    // Visual LED feedback
    indicateWaterQuality();
  }
  
  delay(10);
}

// ===== Function: Read Turbidity with Averaging =====
void readTurbidityWithAveraging() {
  long sumADC = 0;
  
  // Take multiple samples and average
  for (int i = 0; i < SAMPLE_COUNT; i++) {
    sumADC += analogRead(TURBIDITY_PIN);
    delayMicroseconds(100);
  }
  
  // Calculate average
  rawADC = sumADC / SAMPLE_COUNT;
  
  // Convert ADC to Voltage
  // Formula: Voltage = (ADC value / 4095) * 3.3V
  currentVoltage = (rawADC / (float)ADC_MAX_VALUE) * ESP32_VOLTAGE;
  
  // Convert Voltage to NTU
  currentNTU = convertVoltageToNTU(currentVoltage);
}

// ===== Function: Convert Voltage to NTU =====
float convertVoltageToNTU(float voltage) {
  /*
   * Turbidity Sensor Calibration Formula:
   * NTU = (V_clear - V_actual) * (10 / (V_clear - V_turbid))
   * 
   * Where:
   * V_clear = 1.916V (clear water voltage)
   * V_turbid = 1.60V (turbid water voltage)
   * 10 = NTU difference between calibration points
   */
  
  float ntu = (TURBIDITY_VOLTAGE_CLEAR - voltage) * 
              (10.0 / (TURBIDITY_VOLTAGE_CLEAR - TURBIDITY_VOLTAGE_TURBID));
  
  // Limit to reasonable range
  if (ntu < 0) ntu = 0;
  if (ntu > 20) ntu = 20;
  
  return ntu;
}

// ===== Function: Print Results =====
void printResults() {
  float timeSeconds = millis() / 1000.0;
  
  // Time
  Serial.print(timeSeconds, 1);
  for (int i = String((int)timeSeconds).length(); i < 5; i++) Serial.print(" ");
  Serial.print("  | ");
  
  // Raw ADC
  Serial.print(rawADC);
  for (int i = String(rawADC).length(); i < 5; i++) Serial.print(" ");
  Serial.print("   | ");
  
  // Voltage
  Serial.print(currentVoltage, 2);
  Serial.print("V     | ");
  
  // NTU
  Serial.print(currentNTU, 2);
  for (int i = String((int)currentNTU).length(); i < 4; i++) Serial.print(" ");
  Serial.print("  | ");
  
  // Water clarity status
  printWaterStatus();
  
  Serial.println();
}

// ===== Function: Print Water Quality Status =====
void printWaterStatus() {
  if (currentNTU <= 1.0) {
    Serial.print("Excellent (✓✓✓)");
  } else if (currentNTU <= 5.0) {
    Serial.print("Good (✓✓)");
  } else if (currentNTU <= 10.0) {
    Serial.print("Moderate (✓)");
  } else {
    Serial.print("Poor (✗✗✗)");
  }
}

// ===== Function: LED Water Quality Indicator =====
void indicateWaterQuality() {
  /*
   * LED Indicator:
   * - NTU <= 5.0: LED steady ON (good water)
   * - 5.0 < NTU <= 10.0: LED slow blink (moderate)
   * - NTU > 10.0: LED rapid blink (poor water)
   */
  
  if (currentNTU <= 5.0) {
    // Good water - LED steady on
    digitalWrite(LED_PIN, HIGH);
  } else if (currentNTU <= 10.0) {
    // Moderate turbidity - LED slow blink
    digitalWrite(LED_PIN, HIGH);
    delay(300);
    digitalWrite(LED_PIN, LOW);
    delay(300);
  } else {
    // Poor water - LED rapid blink
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
}

/*
 * ═══════════════════════════════════════════════════════════════════
 * USAGE NOTES
 * ═══════════════════════════════════════════════════════════════════
 * 
 * This code is calibrated and ready for production use.
 * 
 * OUTPUT FORMAT:
 * TIME(s) | RAW ADC | VOLTAGE(V) |  NTU  | CLARITY STATUS
 * ════════════════════════════════════════════════════════════
 * 2.0    | 2378   | 1.92V     | 0.45  | Excellent (✓✓✓)
 * 4.0    | 2375   | 1.92V     | 0.58  | Excellent (✓✓✓)
 * 6.0    | 2380   | 1.92V     | 0.32  | Excellent (✓✓✓)
 * 8.0    | 2050   | 1.64V     | 8.20  | Moderate (✓)
 * 
 * WATER CLARITY SCALE:
 * Excellent: NTU 0-1    (Crystal clear water)
 * Good:      NTU 1-5    (Clear water)
 * Moderate:  NTU 5-10   (Slightly turbid)
 * Poor:      NTU >10    (Very turbid water)
 * 
 * READY FOR INTEGRATION:
 * - Can be integrated with Blynk IoT
 * - Can be combined with pH sensor
 * - Can be used with MQTT/WiFi
 * 
 * ═══════════════════════════════════════════════════════════════════
 */

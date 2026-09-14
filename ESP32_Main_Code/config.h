/*
 * Konfigurasi Utama untuk Sistem Monitor Kualiti Air
 * ESP32 DevKit + Sensor Turbidity + Sensor pH + Blynk
 */

#ifndef CONFIG_H
#define CONFIG_H

// ========== WiFi Configuration ==========
#define WIFI_SSID "YOUR_WIFI_NAME"              // Ganti dengan nama WiFi anda
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"      // Ganti dengan password WiFi

// ========== Blynk Configuration ==========
#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"    // Dapatkan dari Blynk app
#define BLYNK_TEMPLATE_NAME "YOUR_TEMPLATE_NAME"
#define BLYNK_AUTH_TOKEN "YOUR_AUTH_TOKEN"      // Dapatkan dari Blynk app

// ========== Pin Configuration ==========
#define TURBIDITY_SENSOR_PIN 34     // GPIO 34 (ADC1_CH6) - Sensor Kekeruhan
#define PH_SENSOR_PIN 35            // GPIO 35 (ADC1_CH7) - Sensor pH
#define LED_PIN 2                   // GPIO 2 - LED Status

// ========== Sensor Calibration ==========
// Turbidity Sensor (Analog Output)
#define TURBIDITY_CLEAN_VOLTAGE 4.2  // Voltan ketika air bersih (mV)
#define TURBIDITY_DIRTY_VOLTAGE 2.3  // Voltan ketika air keruh (mV)

// pH Sensor (E-201-C)
#define PH_CALIBRATION_POINT_1 4.0   // Calibration point 1
#define PH_CALIBRATION_POINT_2 7.0   // Calibration point 2
#define VOLTAGE_AT_PH4 3.0            // Voltase pada pH 4 (mV)
#define VOLTAGE_AT_PH7 2.0            // Voltase pada pH 7 (mV)

// ========== Threshold (Ambang) ==========
#define TURBIDITY_THRESHOLD 5.0      // NTU - Ambang kekeruhan maksimum
#define PH_MIN_THRESHOLD 6.5         // pH minimum
#define PH_MAX_THRESHOLD 8.5         // pH maksimum

// ========== ADC Configuration ==========
#define ADC_RESOLUTION 12             // 12-bit ADC (0-4095)
#define ADC_REFERENCE_VOLTAGE 3.3     // Voltan referensi ESP32 (3.3V)
#define ADC_MAX_VALUE 4095            // Nilai maksimum ADC 12-bit

// ========== Sampling ==========
#define SENSOR_SAMPLE_COUNT 10        // Bilangan sampling untuk averaging
#define SENSOR_READ_INTERVAL 2000     // Interval baca sensor (ms)
#define BLYNK_SEND_INTERVAL 5000      // Interval hantar data ke Blynk (ms)

// ========== Serial Debugging ==========
#define SERIAL_BAUD_RATE 115200      // Baud rate untuk Serial Monitor
#define DEBUG_MODE true               // Enable/disable debug messages

#endif // CONFIG_H

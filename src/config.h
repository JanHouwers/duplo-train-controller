#pragma once

// ── XIAO ESP32S3 Pin Mapping ───────────────────────────
#define POT_PIN         2       // D1 / GPIO2 - Potentiometer
#define BTN_HORN_PIN    3       // D2 / GPIO3 - Horn sound
#define BTN_LED_PIN     4       // D3 / GPIO4 - LED color cycle
#define BTN_DEPART_PIN  5       // D4 / GPIO5 - Station departure sound
#define BTN_WATER_PIN   6       // D5 / GPIO6 - Water refill sound

// ── Potentiometer ──────────────────────────────────────
#define POT_DEADBAND    200     // +/- around midpoint (2048)
#define POT_MID         2048
#define POT_MIN         0
#define POT_MAX         4095

// ── Button Timing (ms) ────────────────────────────────
#define DEBOUNCE_MS     50

// ── Train Speed ────────────────────────────────────────
#define SPEED_MIN       -100
#define SPEED_MAX        100

// ── BLE ────────────────────────────────────────────────
#define BLE_SCAN_DURATION   5       // seconds
#define BLE_RECONNECT_MS    3000    // delay before reconnect attempt

// ── Loop ───────────────────────────────────────────────
#define LOOP_DELAY_MS   10

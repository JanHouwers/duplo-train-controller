#include "ble_train.h"
#include "config.h"
#include <Arduino.h>
#include <Lpf2Hub.h>

// Legoino hub instance (works for Duplo Train Base too)
static Lpf2Hub duploHub;

static uint32_t scanTs       = 0;
static BleState lastBleState = BleState::DISCONNECTED;

// ── Map our LedColor enum to Legoino Color enum ────────

static Color toLegoColor(LedColor c) {
    switch (c) {
        case LedColor::GREEN:  return GREEN;
        case LedColor::RED:    return RED;
        case LedColor::BLUE:   return BLUE;
        case LedColor::YELLOW: return YELLOW;
        case LedColor::ORANGE: return ORANGE;
        case LedColor::PURPLE: return PURPLE;
        case LedColor::CYAN:   return LIGHTBLUE;
        case LedColor::PINK:   return PINK;
        default:               return GREEN;
    }
}

// ── Map our SoundCmd to Legoino DuploTrainBaseSound ────

static byte toLegoSound(SoundCmd cmd) {
    switch (cmd) {
        case SoundCmd::HORN:           return (byte)DuploTrainBaseSound::HORN;
        case SoundCmd::STATION_DEPART: return (byte)DuploTrainBaseSound::STATION_DEPARTURE;
        case SoundCmd::WATER_REFILL:   return (byte)DuploTrainBaseSound::WATER_REFILL;
        default:                       return 0;
    }
}

// ── Sensor Callbacks ───────────────────────────────────

static void colorSensorCallback(void* hub, byte portNumber,
                                 DeviceType deviceType, uint8_t* pData) {
    Lpf2Hub* h = (Lpf2Hub*)hub;
    if (deviceType == DeviceType::DUPLO_TRAIN_BASE_COLOR_SENSOR) {
        int color = h->parseColor(pData);
        Serial.printf("Color sensor: %d\n", color);
    }
}

static void speedometerSensorCallback(void* hub, byte portNumber,
                                       DeviceType deviceType, uint8_t* pData) {
    Lpf2Hub* h = (Lpf2Hub*)hub;
    if (deviceType == DeviceType::DUPLO_TRAIN_BASE_SPEEDOMETER) {
        int speed = h->parseSpeedometer(pData);
        Serial.printf("Speedometer: %d\n", speed);
    }
}

// ── Public API ──────────────────────────────────────────

void bleInit() {
    scanTs = millis() + 1000;  // Small delay before first scan
}

void bleUpdate(TrainState& state) {
    uint32_t now = millis();

    if (duploHub.isConnected()) {
        // ── CONNECTED ──
        if (state.bleState != BleState::CONNECTED) {
            state.bleState = BleState::CONNECTED;
            Serial.println("Duplo Train hub connected!");

            // Set initial LED color
            duploHub.setLedColor(toLegoColor(state.ledColor));

            // Activate sensors
            duploHub.activatePortDevice(
                (byte)DuploTrainHubPort::COLOR, colorSensorCallback);
            duploHub.activatePortDevice(
                (byte)DuploTrainHubPort::SPEEDOMETER, speedometerSensorCallback);
        }

        // Send motor speed if changed
        if (state.speedChanged) {
            state.speedChanged = false;
            duploHub.setBasicMotorSpeed(
                (byte)DuploTrainHubPort::MOTOR, state.speed);
            Serial.printf("Motor speed: %d\n", state.speed);
        }

        // Send LED color if changed
        if (state.colorChanged) {
            state.colorChanged = false;
            duploHub.setLedColor(toLegoColor(state.ledColor));
            Serial.printf("LED color: %s\n", ledColorName(state.ledColor));
        }

        // Play sound if pending
        if (state.soundPending) {
            state.soundPending = false;
            byte snd = toLegoSound(state.soundCmd);
            if (snd != 0) {
                duploHub.playSound(snd);
                Serial.printf("Playing sound: %d\n", snd);
            }
            state.soundCmd = SoundCmd::NONE;
        }
    }
    else if (duploHub.isConnecting()) {
        // ── Hub found by scan, establish connection ──
        if (state.bleState != BleState::CONNECTING) {
            state.bleState = BleState::CONNECTING;
            Serial.println("Duplo hub found, connecting...");
        }
        duploHub.connectHub();
    }
    else {
        // ── Not connected, not connecting ──
        if (state.bleState == BleState::CONNECTED) {
            Serial.println("Duplo hub disconnected!");
            scanTs = now + BLE_RECONNECT_MS;
        }

        // Start a new scan after delay
        if (now >= scanTs) {
            if (state.bleState != BleState::SCANNING) {
                Serial.println("Scanning for Duplo hub...");
            }
            state.bleState = BleState::SCANNING;
            duploHub.init();  // Start async BLE scan
            scanTs = now + (BLE_SCAN_DURATION * 1000) + BLE_RECONNECT_MS;
        }
        else if (state.bleState != BleState::DISCONNECTED &&
                 state.bleState != BleState::SCANNING) {
            state.bleState = BleState::DISCONNECTED;
        }
    }

    if (state.bleState != lastBleState) {
        lastBleState = state.bleState;
    }
}

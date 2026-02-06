#include <Arduino.h>
#include "config.h"
#include "train_state.h"
#include "ble_train.h"

// ── Global State ────────────────────────────────────────
static TrainState trainState;

// ── Potentiometer ───────────────────────────────────────
static int8_t lastSpeed = 0;

static int8_t readPotSpeed() {
    int raw = analogRead(POT_PIN);

    if (raw >= (POT_MID - POT_DEADBAND) && raw <= (POT_MID + POT_DEADBAND)) {
        return 0;  // Deadband = stop
    }

    if (raw < (POT_MID - POT_DEADBAND)) {
        // Backward: 0 -> -100, (MID - DEADBAND) -> 0
        return (int8_t)map(raw, 0, POT_MID - POT_DEADBAND, SPEED_MIN, 0);
    }

    // Forward: (MID + DEADBAND) -> 0, 4095 -> +100
    return (int8_t)map(raw, POT_MID + POT_DEADBAND, POT_MAX, 0, SPEED_MAX);
}

// ── Button Debounce ─────────────────────────────────────

struct DebouncedButton {
    uint8_t  pin;
    bool     lastState;    // last debounced state (HIGH = released)
    uint32_t lastChangeMs; // timestamp of last raw change

    void begin(uint8_t p) {
        pin = p;
        pinMode(pin, INPUT_PULLUP);
        lastState = true;  // released (active LOW)
        lastChangeMs = 0;
    }

    // Returns true on falling edge (press detected)
    bool pressed() {
        bool raw = digitalRead(pin);
        uint32_t now = millis();

        if (raw != lastState) {
            if ((now - lastChangeMs) >= DEBOUNCE_MS) {
                lastChangeMs = now;
                lastState = raw;
                // Falling edge = button pressed (active LOW)
                if (!raw) return true;
            }
        }
        return false;
    }
};

static DebouncedButton btnHorn;
static DebouncedButton btnLed;
static DebouncedButton btnDepart;
static DebouncedButton btnWater;

// ── LED Color Cycle ─────────────────────────────────────
// Cycle: GREEN -> RED -> BLUE -> YELLOW -> ORANGE -> PURPLE -> CYAN -> PINK

static void cycleLedColor() {
    uint8_t next = ((uint8_t)trainState.ledColor + 1) % (uint8_t)LedColor::COUNT;
    trainState.ledColor = (LedColor)next;
    trainState.colorChanged = true;
    Serial.printf("LED -> %s\n", ledColorName(trainState.ledColor));
}

// ── Sound Trigger ───────────────────────────────────────

static void triggerSound(SoundCmd cmd) {
    trainState.soundCmd = cmd;
    trainState.soundPending = true;
}

// ── Setup ───────────────────────────────────────────────

void setup() {
    Serial.begin(115200);
    Serial.println("DUPLO Train Controller starting...");

    // Potentiometer ADC
    analogReadResolution(12);
    pinMode(POT_PIN, INPUT);

    // Buttons
    btnHorn.begin(BTN_HORN_PIN);
    btnLed.begin(BTN_LED_PIN);
    btnDepart.begin(BTN_DEPART_PIN);
    btnWater.begin(BTN_WATER_PIN);

    // BLE
    bleInit();

    Serial.println("Init complete. Press green button on DUPLO hub to pair.");
}

// ── Main Loop ───────────────────────────────────────────

void loop() {
    // 1. Read potentiometer -> speed
    int8_t speed = readPotSpeed();
    if (speed != lastSpeed) {
        lastSpeed = speed;
        trainState.speed = speed;
        trainState.speedChanged = true;
        Serial.printf("Pot speed: %d\n", speed);
    }

    // 2. Check buttons
    if (btnHorn.pressed()) {
        triggerSound(SoundCmd::HORN);
        Serial.println("BTN: Horn");
    }
    if (btnLed.pressed()) {
        cycleLedColor();
    }
    if (btnDepart.pressed()) {
        triggerSound(SoundCmd::STATION_DEPART);
        Serial.println("BTN: Station departure");
    }
    if (btnWater.pressed()) {
        triggerSound(SoundCmd::WATER_REFILL);
        Serial.println("BTN: Water refill");
    }

    // 3. BLE state machine -> send commands if dirty
    bleUpdate(trainState);

    // 4. Yield
    delay(LOOP_DELAY_MS);
}

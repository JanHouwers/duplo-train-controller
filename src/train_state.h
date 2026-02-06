#pragma once
#include <stdint.h>

// LED colors for cycling (subset, ordered per plan)
enum class LedColor : uint8_t {
    GREEN   = 0,
    RED     = 1,
    BLUE    = 2,
    YELLOW  = 3,
    ORANGE  = 4,
    PURPLE  = 5,
    CYAN    = 6,
    PINK    = 7,
    COUNT   = 8
};

// Sound commands for the Duplo Train speaker
enum class SoundCmd : uint8_t {
    NONE            = 0,
    HORN            = 1,
    STATION_DEPART  = 2,
    WATER_REFILL    = 3,
};

// BLE connection state machine
enum class BleState : uint8_t {
    DISCONNECTED,
    SCANNING,
    CONNECTING,
    CONNECTED
};

// Shared state struct - all modules read/write through this
struct TrainState {
    // Motor
    int8_t   speed         = 0;        // -100 to +100

    // LED
    LedColor ledColor      = LedColor::GREEN;

    // Sound
    SoundCmd soundCmd      = SoundCmd::NONE;

    // BLE
    BleState bleState      = BleState::DISCONNECTED;

    // Dirty flags - set by producers, cleared by consumers
    bool     speedChanged  = false;
    bool     colorChanged  = false;
    bool     soundPending  = false;
};

// Human-readable color name for serial debug
inline const char* ledColorName(LedColor c) {
    switch (c) {
        case LedColor::GREEN:  return "GREEN";
        case LedColor::RED:    return "RED";
        case LedColor::BLUE:   return "BLUE";
        case LedColor::YELLOW: return "YELLOW";
        case LedColor::ORANGE: return "ORANGE";
        case LedColor::PURPLE: return "PURPLE";
        case LedColor::CYAN:   return "CYAN";
        case LedColor::PINK:   return "PINK";
        default:               return "???";
    }
}

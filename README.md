# DUPLO Train Controller

Control a LEGO DUPLO Train (hub 10874/10875) from a Seeed XIAO ESP32S3 over BLE. A potentiometer sets motor speed and direction, four buttons trigger sounds and LED color changes.

## Hardware

- [Seeed XIAO ESP32S3](https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/)
- LEGO DUPLO Train Base (hub 10874 or 10875)
- 10K potentiometer
- 4x momentary push buttons

### Wiring

| Pin | GPIO | Function |
|-----|------|----------|
| D1  | 2    | Potentiometer wiper (10K between 3.3V and GND) |
| D2  | 3    | Button — Horn |
| D3  | 4    | Button — LED color cycle |
| D4  | 5    | Button — Station departure sound |
| D5  | 6    | Button — Water refill sound |

Buttons are wired between the pin and GND (internal pull-ups are used).

## Controls

- **Potentiometer** — Center = stop, turn left = reverse, turn right = forward. A deadband around the center prevents drift.
- **Horn button** — Plays the train horn sound.
- **LED button** — Cycles the hub LED through GREEN, RED, BLUE, YELLOW, ORANGE, PURPLE, CYAN, PINK.
- **Station departure button** — Plays the station jingle.
- **Water refill button** — Plays the water refill sound.

## Build & Upload

Requires [PlatformIO](https://platformio.org/).

```bash
# Build
pio run

# Upload (replace COM port as needed)
pio run -t upload --upload-port COM6

# Serial monitor
pio device monitor
```

## Usage

1. Upload firmware to the XIAO.
2. Turn on the DUPLO train — the XIAO will scan and connect automatically via BLE.
3. Use the potentiometer and buttons to control the train.

## Dependencies

- [NimBLE-Arduino](https://github.com/h2zero/NimBLE-Arduino) ^1.4.1
- [Legoino](https://github.com/corneliusmunz/legoino) 1.1.0 (vendored in `lib/` with NimBLE dependency stripped to avoid version conflicts)

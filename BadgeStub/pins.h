#pragma once

#include <stdint.h>

// ---------------------------------------------------------------------------
// pins.h - every GPIO number used by this sketch, in one place
//
// Keeping pin numbers here instead of scattered through the code means a
// wiring change is a one-line edit instead of a search-and-replace. This is
// especially useful when you have multiple revisions of the board, or are
// going from a breadboard prototype to the final hardware.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// RGB LEDs
// ---------------------------------------------------------------------------
// A single data wire feeds a chain of WS2812b style "NeoPixel" LEDs. Only
// one GPIO pin is needed no matter how many LEDs are on the chain, because
// each LED passes the data signal on to the next one in line.
static constexpr uint8_t PIN_RGB_LED = 15;
static constexpr uint16_t NUM_LEDS   = 8;

// ---------------------------------------------------------------------------
// Vibration Motor
// ---------------------------------------------------------------------------
// A small DC vibration motor, the kind found in game controllers and
// phones. It is driven with a plain digital HIGH/LOW signal - HIGH spins
// it, LOW stops it. There is no speed or tone control, just on and off.
static constexpr uint8_t PIN_VIBRATION_MOTOR = 24;

// ---------------------------------------------------------------------------
// Battery Voltage
// ---------------------------------------------------------------------------
// A resistor divider cuts the battery voltage in half so it fits inside the
// 0-3.3V range the ADC can read. A transistor switch connects that divider
// only while a reading is being taken, so it is not draining the battery
// the rest of the time.
static constexpr uint8_t PIN_BATTERY_SENSE    = 27; // ADC input
static constexpr uint8_t PIN_BATTERY_SENSE_EN = 29; // digital output, active HIGH

// ---------------------------------------------------------------------------
// Touchpads
// ---------------------------------------------------------------------------
// Two touch pads (B and E), each using two pins: a "charge" pin that pushes
// current into the pad through a resistor, and a "detect" pin on the far side
// that watches how long the pad takes to reach a logic HIGH. A finger that
// touches the pad adds capacitance, which makes it take measurably longer to
// charge up.  That timing difference is how a touch is detected.
static constexpr uint8_t PIN_TOUCH_1_CHARGE = 21;
static constexpr uint8_t PIN_TOUCH_1_DETECT = 26;
static constexpr uint8_t PIN_TOUCH_2_CHARGE = 23;
static constexpr uint8_t PIN_TOUCH_2_DETECT = 28;

// ---------------------------------------------------------------------------
// IR Receiver
// ---------------------------------------------------------------------------
// A demodulating IR receiver module. It outputs an active-LOW digital
// signal: LOW while it is seeing 38kHz infrared light, HIGH when it isn't.
// Reading the timing of those LOW/HIGH pulses is how an infrared remote
// control (or another badge with an IR transmitter) sends data.
static constexpr uint8_t PIN_IR_RECEIVER = 19;

// ---------------------------------------------------------------------------
// Buttons
// ---------------------------------------------------------------------------
// Three physical push buttons. Each is wired active-LOW: the pin reads
// HIGH normally (pulled up internally) and drops to LOW while the button
// is held down. Left and Right are on the bottom, and A is in between Badge
// and Life on the logo.
static constexpr uint8_t PIN_BTN_LEFT  = 2;
static constexpr uint8_t PIN_BTN_RIGHT = 3;
static constexpr uint8_t PIN_BTN_A     = 1;

// ---------------------------------------------------------------------------
// SAO ports
// ---------------------------------------------------------------------------
// Two Simple Add-On (SAO) v2 headers for plugging in add-on boards. Each
// port has 6 pins: power, ground, an I2C bus (SDA/SCL), and two extra
// general-purpose pins (GPIO1/GPIO2) that an add-on board can use however
// it likes. This sketch does not talk to anything plugged into either
// port, but the pin numbers are mapped out here in case you want to add
// that yourself.
static constexpr uint8_t PIN_SAO_LEFT_SDA   = 12; // I2C0 SDA
static constexpr uint8_t PIN_SAO_LEFT_SCL   = 13; // I2C0 SCL
static constexpr uint8_t PIN_SAO_LEFT_GPIO1 = 10;
static constexpr uint8_t PIN_SAO_LEFT_GPIO2 = 11;

static constexpr uint8_t PIN_SAO_RIGHT_SDA   = 6; // I2C1 SDA
static constexpr uint8_t PIN_SAO_RIGHT_SCL   = 7; // I2C1 SCL
static constexpr uint8_t PIN_SAO_RIGHT_GPIO1 = 4;
static constexpr uint8_t PIN_SAO_RIGHT_GPIO2 = 5;

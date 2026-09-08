// ===========================================================================
// BadgelifeExample Firmware
// a small example sketch for the Badgelife Village Badge (DC34, 2026)
//
// This code demonstrates the badge's hardware to get you started. It's got:
//   - 8 RGB LEDs, running a rainbow animation
//   - A vibration motor, buzzing at boot and then once every 5 seconds
//   - A battery voltage sense circuit, printed to the serial monitor
//   - Two capacitive touchpads, each flashing a color while held
//   - An infrared receiver, printing whatever remote control code it hears
//   - Three tactile buttons, printing press/double-press/long-press events
// ===========================================================================

#include <Adafruit_NeoPixel.h>
#include <IRremote.hpp>
#include <BfButton.h>
#include "pins.h"
#include "vibration.h"
#include "battery.h"
#include "touch.h"

// NEO_GRB + NEO_KHZ800 matches the WS2812 LEDs on this badge: NEO_GRB is
// the order the three color bytes are sent in, and NEO_KHZ800 is the data
// rate the LEDs expect.  Do not change this!
Adafruit_NeoPixel strip(NUM_LEDS, PIN_RGB_LED, NEO_GRB + NEO_KHZ800);

// One BfButton object per physical button. STANDALONE_DIGITAL tells the
// library this is a plain digital button (as opposed to several buttons
// sharing one analog pin, which BfButton also supports but this badge
// does not need). The buttons are wired active-LOW with the pin idling
// HIGH, which is what the last two constructor arguments (pullup=true,
// buttonLogic=LOW) tell the library to expect. Left and Right are on the
// bottom, and A is inbetween Badge and Life on the logo.
BfButton btnLeft(BfButton::STANDALONE_DIGITAL, PIN_BTN_LEFT, true, LOW);
BfButton btnRight(BfButton::STANDALONE_DIGITAL, PIN_BTN_RIGHT, true, LOW);
BfButton btnA(BfButton::STANDALONE_DIGITAL, PIN_BTN_A, true, LOW);

// One handler shared by all three buttons. BfButton always passes back
// which button object triggered it (btn) and which pattern was detected
// (pattern), so a single function can tell them apart instead of writing
// a separate function per button.
static void OnButtonPress(BfButton *btn, BfButton::press_pattern_t pattern)
{
    const char *name = "Unknown";
    if (btn == &btnLeft)  name = "Left";
    if (btn == &btnRight) name = "Right";
    if (btn == &btnA)     name = "A";

    switch (pattern) {
        case BfButton::SINGLE_PRESS:
            Serial.print(name);
            Serial.println(" button pressed");
            break;
        case BfButton::DOUBLE_PRESS:
            Serial.print(name);
            Serial.println(" button double-pressed");
            break;
        case BfButton::LONG_PRESS:
            Serial.print(name);
            Serial.println(" button held down");
            // Button A held down triggers a longer buzz
            if (btn == &btnA) {
                Vibration_Pulse(300);
            }
            break;
    }

    // A quick tap of any button also gives a short buzz, so you can feel
    // the badge react even without watching the serial monitor.
    if (pattern == BfButton::SINGLE_PRESS) {
        Vibration_Pulse(30);
    }
}

// How often (in milliseconds) to buzz the vibration motor, just to prove it
// is wired up and working.
static constexpr uint32_t VIBRATION_INTERVAL_MS = 5000;
static constexpr uint16_t VIBRATION_PULSE_MS     = 100;

// How often to print the battery voltage to the serial monitor.
static constexpr uint32_t BATTERY_PRINT_INTERVAL_MS = 2000;

static uint32_t s_lastVibrationMs = 0;
static uint32_t s_lastBatteryPrintMs = 0;

void setup()
{
    // USB serial, for the printouts below. If nothing is listening on the
    // other end this simply has nowhere to send its output, which is
    // harmless - the rest of the sketch does not depend on it.
    Serial.begin(115200);

    strip.begin();
    // 8 LEDs at full brightness white draws close to 500mA, which is a
    // lot to ask of a small battery. 40 keeps this demo modest.
    strip.setBrightness(40);
    strip.clear();
    strip.show();

    Vibration_Init();
    Battery_Init();
    Touch_Init();

    // IrReceiver is a global object provided by the IRremote library.
    // begin() starts listening on PIN_IR_RECEIVER; passing 0 for the
    // feedback LED pin means "no feedback LED" (the badge does not have
    // one wired to a spare pin for this purpose).
    IrReceiver.begin(PIN_IR_RECEIVER, 0);

    // Wire up all three press patterns for each button. onDoublePress()
    // and onPressFor() both take an optional timeout in milliseconds -
    // the defaults (300ms to catch a second tap, 3000ms to count as a
    // long press) are used here.
    btnLeft.onPress(OnButtonPress).onDoublePress(OnButtonPress).onPressFor(OnButtonPress);
    btnRight.onPress(OnButtonPress).onDoublePress(OnButtonPress).onPressFor(OnButtonPress);
    btnA.onPress(OnButtonPress).onDoublePress(OnButtonPress).onPressFor(OnButtonPress);

    Serial.println("BadgelifeExample booting up");
    Serial.print("Battery reads ");
    Serial.print(Battery_Millivolts());
    Serial.println(" mV");

    // One short buzz at boot, so you know the firmware made it to setup()
    // even before the serial monitor is connected.
    Vibration_Pulse(VIBRATION_PULSE_MS);
    s_lastVibrationMs = millis();
    s_lastBatteryPrintMs = millis();
}

void loop()
{
    uint32_t now = millis();

    // -----------------------------------------------------------------------
    // RGB LEDs
    // -----------------------------------------------------------------------
    // Adafruit_NeoPixel::rainbow() fills the whole strip with one cycle of
    // the color wheel, offset over time by first_hue so it slides forward
    // every frame. The hue value wraps around after 65535, which is why
    // this is safe to run forever without ever resetting it.
    uint16_t firstHue = (uint16_t)(now * 40);
    strip.rainbow(firstHue);
    strip.show();

    // -----------------------------------------------------------------------
    // Vibration Motor
    // -----------------------------------------------------------------------
    // Vibration_Pump() must run every loop() so it can notice when a pulse
    // that Vibration_Pulse() started is due to turn back off.
    Vibration_Pump();
    if (now - s_lastVibrationMs >= VIBRATION_INTERVAL_MS) {
        Vibration_Pulse(VIBRATION_PULSE_MS);
        s_lastVibrationMs = now;
    }

    // -----------------------------------------------------------------------
    // Battery Voltage
    // -----------------------------------------------------------------------
    Battery_Pump();
    if (now - s_lastBatteryPrintMs >= BATTERY_PRINT_INTERVAL_MS) {
        Serial.print("Battery: ");
        Serial.print(Battery_Millivolts());
        Serial.println(" mV");
        s_lastBatteryPrintMs = now;
    }

    // -----------------------------------------------------------------------
    // Buttons
    // -----------------------------------------------------------------------
    btnLeft.read();
    btnRight.read();
    btnA.read();

    // -----------------------------------------------------------------------
    // Touchpads
    // -----------------------------------------------------------------------
    Touch_Pump();
    if (Touch_IsTouched(TOUCH_PAD_1)) {
        Serial.println("Touchpad B Pressed");
        strip.setPixelColor(0, strip.Color(255, 0, 0)); // red
        strip.show();
    }
    if (Touch_IsTouched(TOUCH_PAD_2)) {
        Serial.println("Touchpad E Pressed");
        strip.setPixelColor(1, strip.Color(0, 0, 255)); // blue
        strip.show();
    }

    // -----------------------------------------------------------------------
    // IR Receiver
    // -----------------------------------------------------------------------
    // IrReceiver.decode() returns true once a full signal has been
    // received and decoded. The decoded fields (protocol, address,
    // command) are then available in IrReceiver.decodedIRData until
    // resume() is called, which clears them and starts listening for
    // the next signal.
    if (IrReceiver.decode()) {
        Serial.print("IR command received: 0x");
        Serial.println(IrReceiver.decodedIRData.command, HEX);
        IrReceiver.resume();
    }
}

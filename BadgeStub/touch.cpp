#include "touch.h"
#include "pins.h"
#include <Arduino.h>

struct TouchPad {
    uint8_t chargePin;
    uint8_t detectPin;
};

static const TouchPad PADS[TOUCH_PAD_COUNT] = {
    { PIN_TOUCH_1_CHARGE, PIN_TOUCH_1_DETECT },
    { PIN_TOUCH_2_CHARGE, PIN_TOUCH_2_DETECT },
};

// Upper limit on how many loop iterations to spend waiting for the detect
// pin to go HIGH. If a pad were unplugged or miswired, the detect pin
// might never rise on its own - this keeps ReadRaw() from hanging forever
// in that case.
static constexpr uint32_t CHARGE_TIMEOUT_LOOPS = 20000;

// A touch is reported once a reading climbs more than this many percent
// above the untouched baseline. A finger typically adds enough
// capacitance to noticeably slow the charge time, so 40% leaves headroom
// above ordinary sensor noise while still being easy to trigger on
// purpose. Raise this if the pad falsely triggers on its own; lower it if
// a real touch is not being noticed.
static constexpr uint32_t TOUCH_THRESHOLD_PERCENT = 40;

// Require this many consecutive readings on the other side of the
// threshold before actually changing state. Without this, a single noisy
// reading could report a touch (or a release) that was not really there.
static constexpr uint8_t DEBOUNCE_COUNT = 2;

static uint32_t s_baseline[TOUCH_PAD_COUNT];
static bool     s_touched[TOUCH_PAD_COUNT];
static uint8_t  s_debounce[TOUCH_PAD_COUNT];

// Charges the pad and counts how many loop iterations it takes for the
// detect pin to see the charge arrive. A bigger count means the pad took
// longer to charge, which means more capacitance is on it right now.
static uint32_t ReadRaw(uint8_t pad)
{
    pinMode(PADS[pad].detectPin, INPUT);
    digitalWrite(PADS[pad].chargePin, LOW);
    delayMicroseconds(10); // fully discharge the pad before timing it

    digitalWrite(PADS[pad].chargePin, HIGH);
    uint32_t count = 0;
    while (digitalRead(PADS[pad].detectPin) == LOW && count < CHARGE_TIMEOUT_LOOPS) {
        count++;
    }

    digitalWrite(PADS[pad].chargePin, LOW);
    return count;
}

void Touch_Init()
{
    for (uint8_t pad = 0; pad < TOUCH_PAD_COUNT; pad++) {
        pinMode(PADS[pad].chargePin, OUTPUT);
        digitalWrite(PADS[pad].chargePin, LOW);

        // Average several readings taken while nothing should be
        // touching the pad, so later readings have a baseline to compare
        // against. Every pad charges at a slightly different speed
        // depending on its wiring, so this baseline is measured per pad
        // rather than assumed to be some fixed number.
        const uint8_t samples = 16;
        uint32_t total = 0;
        for (uint8_t i = 0; i < samples; i++) {
            total += ReadRaw(pad);
            delayMicroseconds(50);
        }
        s_baseline[pad] = total / samples;
        s_touched[pad]  = false;
        s_debounce[pad] = 0;
    }
}

void Touch_Pump()
{
    for (uint8_t pad = 0; pad < TOUCH_PAD_COUNT; pad++) {
        uint32_t raw = ReadRaw(pad);
        uint32_t threshold = s_baseline[pad] + (s_baseline[pad] * TOUCH_THRESHOLD_PERCENT) / 100;
        bool aboveThreshold = raw > threshold;

        if (aboveThreshold != s_touched[pad]) {
            s_debounce[pad]++;
            if (s_debounce[pad] >= DEBOUNCE_COUNT) {
                s_touched[pad] = aboveThreshold;
                s_debounce[pad] = 0;
            }
        } else {
            s_debounce[pad] = 0;
        }
    }
}

bool Touch_IsTouched(uint8_t pad)
{
    if (pad >= TOUCH_PAD_COUNT) {
        return false;
    }
    return s_touched[pad];
}

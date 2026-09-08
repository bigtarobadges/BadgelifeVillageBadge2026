#include "battery.h"
#include "pins.h"
#include <Arduino.h>

// --- The math behind this file -----------------------------------------
//
// The divider uses two equal resistors, so the ADC pin sees exactly half
// of the battery voltage. To recover the real battery voltage, read the
// ADC, convert that raw number to millivolts, then double it.
//
// ADC_REF_MV and ADC_MAX describe the ADC itself: this board runs the ADC
// off the 3.3V rail (ADC_REF_MV) with 12 bits of resolution, so readings
// range from 0 to 4095 (ADC_MAX) across that 0-3300mV span.
static constexpr uint32_t ADC_REF_MV = 3300;
static constexpr uint32_t ADC_MAX    = 4095;
static constexpr uint8_t  ADC_BITS   = 12;

// How many ADC samples to average together. A few cheap readings smooth
// out electrical noise better than trusting a single sample.
static constexpr uint8_t OVERSAMPLE = 4;

// How long to wait after enabling the divider before trusting the reading.
// The resistors and a small capacitor on the divider need a moment to
// settle to their final voltage after being switched on - reading too soon
// would measure a value partway there instead of the real voltage.
static constexpr uint32_t SETTLE_US = 5000; // 5ms

// Re-check the battery this often. A battery's voltage does not change
// quickly, so there is no need to sample it on every single loop() call -
// that would just spend extra time with the divider switched on.
static constexpr uint32_t SAMPLE_INTERVAL_MS = 5000;

static uint16_t s_millivolts   = 0;
static uint32_t s_lastSampleMs = 0;

static void Sample()
{
    // Turn the divider on, wait for it to settle, average a few readings,
    // then turn the divider back off so it stops drawing current.
    digitalWrite(PIN_BATTERY_SENSE_EN, HIGH);
    delayMicroseconds(SETTLE_US);

    uint32_t total = 0;
    for (uint8_t i = 0; i < OVERSAMPLE; i++) {
        total += analogRead(PIN_BATTERY_SENSE);
    }

    digitalWrite(PIN_BATTERY_SENSE_EN, LOW);

    uint32_t rawAverage = total / OVERSAMPLE;

    // Step 1: convert the raw ADC count to the millivolts the pin actually
    // saw (0..3300mV range mapped from 0..4095 counts).
    uint32_t senseMv = (rawAverage * ADC_REF_MV) / ADC_MAX;

    // Step 2: the divider cut the real voltage in half, so double it back.
    s_millivolts = (uint16_t)(senseMv * 2);
}

void Battery_Init()
{
    pinMode(PIN_BATTERY_SENSE_EN, OUTPUT);
    digitalWrite(PIN_BATTERY_SENSE_EN, LOW);
    pinMode(PIN_BATTERY_SENSE, INPUT);
    analogReadResolution(ADC_BITS);

    // Take one reading right away so a demo printing the voltage at boot
    // has a real number instead of zero.
    Sample();
    s_lastSampleMs = millis();
}

void Battery_Pump()
{
    uint32_t now = millis();
    if ((now - s_lastSampleMs) < SAMPLE_INTERVAL_MS) {
        return;
    }
    Sample();
    s_lastSampleMs = now;
}

uint16_t Battery_Millivolts()
{
    return s_millivolts;
}

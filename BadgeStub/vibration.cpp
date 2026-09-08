#include "vibration.h"
#include "pins.h"
#include <Arduino.h>

// 0 means "no pulse scheduled". millis() also starts at 0 at boot, but the
// very first loop() call is always pump-ing a motor that Vibration_Init()
// already left LOW with no pulse pending, so there is no meaningful chance
// of confusing "just booted" with "pulse due right now".
static uint32_t s_motorOffAtMs = 0;

void Vibration_Init()
{
    pinMode(PIN_VIBRATION_MOTOR, OUTPUT);
    digitalWrite(PIN_VIBRATION_MOTOR, LOW);
    s_motorOffAtMs = 0;
}

void Vibration_Pulse(uint16_t durationMs)
{
    digitalWrite(PIN_VIBRATION_MOTOR, HIGH);
    s_motorOffAtMs = millis() + durationMs;
}

void Vibration_Pump()
{
    if (s_motorOffAtMs != 0 && millis() >= s_motorOffAtMs) {
        digitalWrite(PIN_VIBRATION_MOTOR, LOW);
        s_motorOffAtMs = 0;
    }
}

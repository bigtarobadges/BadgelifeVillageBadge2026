#pragma once

#include <stdint.h>

// ---------------------------------------------------------------------------
// vibration.h - vibration motor driver
//
// The motor is a plain DC vibration motor, driven with digital HIGH/LOW.
// There is no PWM speed control here.  A HIGH is "on" at full strength, and 
// A LOW is "off". Timed pulses are non-blocking: call Vibration_Pulse() to
// start one, then keep calling Vibration_Pump() every loop() so it can turn
// the motor back off at the right time without a delay() freezing everything
// else (like the rainbow animation).
//
// Usage:
//   Vibration_Init()                  - once, in setup()
//   Vibration_Pulse(durationMs)       - start a timed buzz
//   Vibration_Pump()                  - every loop(), turns the motor off
//                                       once the pulse duration has elapsed
// ---------------------------------------------------------------------------

void Vibration_Init();

// Start the motor and schedule it to turn off automatically after
// durationMs milliseconds. Calling this again while a pulse is already
// running just restarts the timer - it does not stack up multiple buzzes.
void Vibration_Pulse(uint16_t durationMs);

// Must be called every loop() iteration. Cheap to call - it just checks a
// timestamp and turns the pin off if a scheduled pulse has finished.
void Vibration_Pump();

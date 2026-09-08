#pragma once

#include <stdint.h>

// ---------------------------------------------------------------------------
// battery.h - reads the battery voltage through a gated resistor divider
//
// The badge splits the battery voltage in half with two equal resistors
// before it reaches the ADC pin (the RP2350's ADC can only read 0-3.3V, and
// a charged LiPo can be over 4V). A transistor switch connects that divider
// to the battery only while a reading is being taken, so it does not slowly
// drain the battery the rest of the time.
//
// Usage:
//   Battery_Init()         - once, in setup(); takes a first reading
//   Battery_Pump()         - every loop(); re-samples every few seconds
//   Battery_Millivolts()   - most recent reading, in millivolts
// ---------------------------------------------------------------------------

void Battery_Init();
void Battery_Pump();

// Most recent battery voltage, in millivolts (thousandths of a volt). For
// example a fresh LiPo reads back somewhere around 4000-4200.
uint16_t Battery_Millivolts();

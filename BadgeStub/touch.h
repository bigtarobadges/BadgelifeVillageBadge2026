#pragma once

#include <stdint.h>
#include <stdbool.h>

// ---------------------------------------------------------------------------
// touch.h - reads the capacitive touchpads
//
// Each pad uses two pins: a "charge" pin that pushes current into the pad
// through a resistor, and a "detect" pin on the far side that watches how
// long the pad takes to reach a logic HIGH. Touching the pad adds
// capacitance, which slows that charging down. A longer time means a touch
// happened.
//
// Usage:
//   Touch_Init()             - measures an untouched baseline for each pad
//   Touch_Pump()             - every loop(); takes a fresh reading
//   Touch_IsTouched(pad)     - true while that pad is currently touched
// ---------------------------------------------------------------------------

enum {
    TOUCH_PAD_1 = 0,
    TOUCH_PAD_2 = 1,
    TOUCH_PAD_COUNT = 2,
};

void Touch_Init();
void Touch_Pump();

// True while pad is being touched right now. pad is TOUCH_PAD_1 or
// TOUCH_PAD_2. Out of range values return false.
bool Touch_IsTouched(uint8_t pad);

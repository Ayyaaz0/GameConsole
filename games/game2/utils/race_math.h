#ifndef RACE_MATH_H
#define RACE_MATH_H

#include <stdint.h>

// Clamp a float value between a minimum and maximum
float RaceMath_ClampFloat(float value, float min_value, float max_value);

// Clamp a signed 32-bit integer between a minimum and maximum
int32_t RaceMath_ClampInt32(int32_t value, int32_t min_value,
                            int32_t max_value);

// Wrap an unsigned 16-bit value so it stays inside a repeating range
uint16_t RaceMath_WrapU16(uint16_t value, uint16_t period);

#endif
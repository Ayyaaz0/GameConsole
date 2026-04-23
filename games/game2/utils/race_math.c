#include "race_math.h"

float RaceMath_ClampFloat(float value, float min_value, float max_value) {
  if (value < min_value) {
    return min_value;
  }

  if (value > max_value) {
    return max_value;
  }

  return value;
}

int32_t RaceMath_ClampInt32(int32_t value, int32_t min_value,
                            int32_t max_value) {
  if (value < min_value) {
    return min_value;
  }

  if (value > max_value) {
    return max_value;
  }

  return value;
}

uint16_t RaceMath_WrapU16(uint16_t value, uint16_t period) {
  if (period == 0U) {
    return 0U;
  }

  return (uint16_t)(value % period);
}
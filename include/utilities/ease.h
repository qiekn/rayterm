#pragma once

#include <raylib.h>
#include <cmath>

float EaseOutCubic(float t) { return 1 - pow(1 - t, 3); }

float EaseOutQuad(float t) { return 1 - (1 - t) * (1 - t); }

float LerpAngle(float a, float b, float t) {
  float diff = b - a;
  if (diff > PI) diff -= 2 * PI;
  if (diff < -PI) diff += 2 * PI;
  return a + diff * t;
}

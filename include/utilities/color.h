#pragma once

#include <raylib.h>
#include <string>

inline Color Hexc(const std::string& color) {
  if (color.size() != 7 || color[0] != '#') {
    TraceLog(LOG_ERROR, "Invalid color format: %s", color.c_str());
    return WHITE;
  }

  try {
    // Convert hex string (without #) to integer
    unsigned int num = std::stoul(color.substr(1), nullptr, 16);
    // Use Raylib's GetColor
    return GetColor(num << 8 | 0xFF);  // Shift to RRGGBBAA format
  } catch (const std::exception& e) {
    TraceLog(LOG_ERROR, "Invalid hex color: %s", color.c_str());
    return WHITE;
  }
}

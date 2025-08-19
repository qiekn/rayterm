#include "terminal.h"
#include <raylib.h>
#include <cstdlib>
#include "managers/font-manager.h"

Terminal::Terminal() {}

Terminal::~Terminal() {}

void Terminal::Update(float dt) {
  HandleInput();

  // Toggle Animation
  float target_width = is_open_ ? GetScreenWidth() : 0.0f;
  if (current_panel_width_ != target_width) {
    float diff = target_width - current_panel_width_;
    float step = animation_speed_ * dt;
    if (abs(diff) <= step) {
      current_panel_width_ = target_width;
    } else {
      current_panel_width_ += diff > 0 ? step : -step;
    }
  }
}

void Terminal::Draw() {
  float panel_x = GetScreenWidth() - current_panel_width_;

  // Background
  DrawRectangle(panel_x, 0, current_panel_width_, GetScreenHeight(), (Color){40, 40, 40, 240});
  DrawRectangleLines(panel_x, 0, current_panel_width_, GetScreenHeight(), WHITE);

  {  // Title
    const char* title_text = "Terminal";
    Vector2 text_size = MeasureTextEx(FontManager::Get().Italic(), title_text, FontSize::kTitle, 2.0f);
    float x = panel_x + (current_panel_width_ - text_size.x) / 2.0f;
    float y = 20.0f;
    if (current_panel_width_ <= text_size.x) {
      x = GetScreenWidth() - current_panel_width_;
    }
    DrawTextEx(FontManager::Get().Italic(), title_text, {x, y}, FontSize::kTitle, 2.0f, WHITE);
  }
}

void Terminal::HandleInput() {
  if (IsKeyPressed(KEY_BACKSLASH)) {
    TraceLog(LOG_WARNING, "Terminal toggle by backspace");
    Toggle();
  }
}

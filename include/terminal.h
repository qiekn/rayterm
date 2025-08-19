#pragma once

#include <raylib.h>

class Terminal {
public:
  Terminal();
  virtual ~Terminal();

  void Update(float dt);
  void Draw();

  void Toggle() {
    is_open_ = !is_open_;
    TraceLog(LOG_WARNING, "Terminal %s", is_open_ ? "opened" : "closed");
  }

  bool IsOpen() { return is_open_; }

private:
  bool is_open_{false};

  /* UI */
  float current_panel_width_{0.0f};
  float animation_speed_{3000.0f};

  void HandleInput();
};

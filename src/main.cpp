#include "managers/font-manager.h"
#include "raylib.h"
#include "terminal.h"

int main() {
  SetTraceLogLevel(LOG_WARNING);
  SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI);

  const int screen_width = 1000;
  const int screen_height = 700;

  InitWindow(screen_width, screen_height, "game");
  SetTargetFPS(120);

  Terminal terminal;

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();

    /*─────────────────────────────────────┐
    │                Update                │
    └──────────────────────────────────────*/
    terminal.Update(dt);

    /*─────────────────────────────────────┐
    │                Draw                  │
    └──────────────────────────────────────*/
    BeginDrawing();

    // 动态背景
    Color background_color = {20, 30, 50, 255};
    ClearBackground(background_color);

    // 绘制标题
    DrawTextEx(FontManager::Get().Italic(), "A Great Game", (Vector2){50, 50}, static_cast<float>(FontSize::kTitle),
               2.0f, WHITE);
    DrawTextEx(FontManager::Get().Italic(), "Press [\\] to togge terminal", (Vector2){50, 100},
               static_cast<float>(FontSize::kSubtitle), 2.0f, LIGHTGRAY);
    DrawTextEx(FontManager::Get().Italic(), "Press [ESC] to exit", (Vector2){50, 130},
               static_cast<float>(FontSize::kSubtitle), 2.0f, LIGHTGRAY);

    terminal.Draw();
    EndDrawing();
  }

  CloseWindow();
  return 0;
}

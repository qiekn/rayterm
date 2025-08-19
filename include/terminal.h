#pragma once
#include <raylib.h>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

class Terminal {
public:
  using CommandFunc = std::function<void(const std::vector<std::string>&)>;

  Terminal();
  virtual ~Terminal();
  void Update(float dt);
  void Draw();
  void Toggle() { is_open_ = !is_open_; }
  bool IsOpen() { return is_open_; }

private:
  bool is_open_{false};

  /* UI */
  float current_panel_width_{0.0f};
  float max_panel_width_{GetScreenWidth() * 0.7f};  // 70% of screen width
  float animation_speed_{3000.0f};

  /* Shell functionality */
  std::string current_input_;
  std::vector<std::string> output_history_;
  std::vector<std::string> command_history_;
  int history_index_{-1};
  std::string current_directory_;
  std::map<std::string, std::vector<std::string>> virtual_filesystem_;
  std::map<std::string, std::string> virtual_files_;

  /* UI state */
  float scroll_offset_{0.0f};
  bool cursor_visible_{true};
  float cursor_timer_{0.0f};
  const float cursor_blink_time_{0.5f};
  float font_size_{16.0f};  // Default font size for content area
  Color background_color_{(Color){20, 20, 20, 240}};

  /* Backspace handling */
  bool backspace_held_{false};
  float backspace_timer_{0.0f};
  float backspace_initial_delay_{0.5f};
  float backspace_repeat_rate_{0.05f};

  void HandleInput();                // 处理用户输入
  void HandleCursorBlink(float dt);  // 更新光标闪烁
  void HandleBackspace(float dt);    // 处理长按删除
  void UpdateAnimation(float dt);    // 更新划入滑出动画

  void ProcessCommand(const std::string& command);
  void AddOutput(const std::string& text);
  void InitializeFilesystem();

  /* Commands */
  void CmdLs(const std::vector<std::string>& args);
  void CmdCd(const std::vector<std::string>& args);
  void CmdCat(const std::vector<std::string>& args);
  void CmdEcho(const std::vector<std::string>& args);
  void CmdHelp(const std::vector<std::string>& args);
  void CmdClear(const std::vector<std::string>& args);
  void CmdPwd(const std::vector<std::string>& args);
  void CmdSet(const std::vector<std::string>& args);
  std::unordered_map<std::string, CommandFunc> command_table_;

  /* Utilities */
  std::vector<std::string> SplitCommand(const std::string& command);
  std::string JoinPath(const std::string& base, const std::string& path);
  std::string ResolvePath(const std::string& path);
  bool PathExists(const std::string& path);
  bool IsDirectory(const std::string& path);
};

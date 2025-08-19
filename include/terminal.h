#pragma once
#include <raylib.h>
#include <map>
#include <string>
#include <vector>

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

  /* Backspace handling */
  bool backspace_held_{false};
  float backspace_timer_{0.0f};
  float backspace_initial_delay_{0.5f};
  float backspace_repeat_rate_{0.05f};

  void HandleInput();
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

  /* Utilities */
  std::vector<std::string> SplitCommand(const std::string& command);
  std::string JoinPath(const std::string& base, const std::string& path);
  std::string ResolvePath(const std::string& path);
  bool PathExists(const std::string& path);
  bool IsDirectory(const std::string& path);
};

#include "terminal.h"
#include <raylib.h>
#include <algorithm>
#include <cstdlib>
#include <sstream>
#include "managers/font-manager.h"
#include "utilities/color.h"

Terminal::Terminal() : current_directory_("/home/player") {
  InitializeFilesystem();
  AddOutput("Welcome to Game Terminal v1.0");
  AddOutput("Type 'help' for available commands");
  AddOutput("");
}

Terminal::~Terminal() {}

void Terminal::Update(float dt) {
  HandleInput();
  HandleBackspace(dt);
  HandleCursorBlink(dt);
  UpdateAnimation(dt);
}

// clang-format off
void Terminal::Draw() {
  if (current_panel_width_ <= 0.0f) return;

  float panel_x = GetScreenWidth() - current_panel_width_;
  float panel_y = 0.0f;
  float panel_height = GetScreenHeight();
  float line_height = font_size_ + 4.0f;  // Add some padding for line height

  // Background
  DrawRectangle(panel_x, panel_y, current_panel_width_, panel_height, background_color_);
  DrawRectangleLines(panel_x, panel_y, current_panel_width_, panel_height, (Color){100, 100, 100, 255});

  // Title
  const char* title_text = "Game Terminal";
  Vector2 title_text_size = MeasureTextEx(FontManager::Get().Italic(), title_text, FontSize::kTitle, 1.0f);
  float title_x_offset = (max_panel_width_ - title_text_size.x) / 2.0f;
  float title_y = 10.0f;

  if (panel_x >= title_x_offset) {
    DrawTextEx(FontManager::Get().Italic(), title_text, {panel_x + title_x_offset, title_y},
               FontSize::kTitle, 1.0f, WHITE);
  }

  // Terminal content area
  float content_y = title_y + title_text_size.y + 20.0f;
  float content_height = panel_height - content_y - 40.0f;  // Leave space for current input

  // Draw output history
  float y_offset = content_y - scroll_offset_;

  for (const auto& line : output_history_) {
    if (y_offset > content_y - line_height && y_offset < content_y + content_height) {
      Color text_color = GREEN;
      // Different colors for different types of text
      if (line.find("$") != std::string::npos && line.find("/") != std::string::npos) {
        text_color = YELLOW;  // Command lines
      } else if (line.find("bash:") != std::string::npos ||
                 line.find("cannot access") != std::string::npos) {
        text_color = RED;  // Error messages
      }

      DrawTextEx(FontManager::Get().Mono(), line.c_str(), {panel_x + 10.0f, y_offset},
                 font_size_, 1.0f, text_color);
    }
    y_offset += line_height;
  }

  // Draw current input line at bottom of history
  if (y_offset > content_y - line_height && y_offset < content_y + content_height + line_height) {
    std::string prompt = current_directory_ + "$ ";
    std::string input_line = prompt + current_input_;

    DrawTextEx(FontManager::Get().Mono(), input_line.c_str(), {panel_x + 10.0f, y_offset},
               font_size_, 1.0f, WHITE);

    // Draw cursor
    if (cursor_visible_) {
      Vector2 prompt_size =
          MeasureTextEx(FontManager::Get().Mono(), input_line.c_str(), font_size_, 1.0f);
      DrawRectangle(panel_x + 10.0f + prompt_size.x, y_offset, 2.0f, font_size_, WHITE);
    }
  }
}
// clang-format on

void Terminal::HandleInput() {
  if (!is_open_) {
    if (IsKeyPressed(KEY_BACKSLASH)) {
      Toggle();
    }
    return;
  }

  if (IsKeyPressed(KEY_BACKSLASH)) {
    Toggle();
    return;
  }

  // Handle text input
  int key = GetCharPressed();
  while (key > 0) {
    if (key >= 32 && key <= 126) {  // Printable characters
      current_input_ += static_cast<char>(key);
    }
    key = GetCharPressed();
  }

  // Handle backspace with repeat functionality
  if (IsKeyPressed(KEY_BACKSPACE)) {
    if (!current_input_.empty()) {
      current_input_.pop_back();
    }
    backspace_held_ = true;
    backspace_timer_ = 0.0f;
  }

  if (IsKeyReleased(KEY_BACKSPACE)) {
    backspace_held_ = false;
    backspace_timer_ = 0.0f;
  }

  if (IsKeyPressed(KEY_ENTER)) {
    // Add the current command line to history
    std::string command_line = current_directory_ + "$ " + current_input_;
    AddOutput(command_line);

    if (!current_input_.empty()) {
      command_history_.push_back(current_input_);
      ProcessCommand(current_input_);
    }

    current_input_.clear();
    history_index_ = -1;
  }

  // Command history navigation
  if (IsKeyPressed(KEY_UP) && !command_history_.empty()) {
    if (history_index_ == -1) {
      history_index_ = command_history_.size() - 1;
    } else if (history_index_ > 0) {
      history_index_--;
    }
    if (history_index_ >= 0) {
      current_input_ = command_history_[history_index_];
    }
  }

  if (IsKeyPressed(KEY_DOWN) && history_index_ != -1) {
    history_index_++;
    if (history_index_ >= command_history_.size()) {
      history_index_ = -1;
      current_input_.clear();
    } else {
      current_input_ = command_history_[history_index_];
    }
  }

  // Scroll handling
  float wheel = GetMouseWheelMove();
  if (wheel != 0.0f) {
    scroll_offset_ -= wheel * 60.0f;
    scroll_offset_ = fmax(0.0f, scroll_offset_);
  }
}

// Handle backspace repeat
void Terminal::HandleBackspace(float dt) {
  if (backspace_held_) {
    backspace_timer_ += dt;
    if (backspace_timer_ >= backspace_initial_delay_) {
      // After initial delay, repeat at faster rate
      if (fmod(backspace_timer_ - backspace_initial_delay_, backspace_repeat_rate_) < dt &&
          !current_input_.empty()) {
        current_input_.pop_back();
      }
    }
  }
}

// Update cursor blink
void Terminal::HandleCursorBlink(float dt) {
  cursor_timer_ += dt;
  if (cursor_timer_ >= cursor_blink_time_) {
    cursor_visible_ = !cursor_visible_;
    cursor_timer_ = 0.0f;
  }
}

// Toggle Animation
void Terminal::UpdateAnimation(float dt) {
  float target_width = is_open_ ? max_panel_width_ : 0.0f;
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

void Terminal::ProcessCommand(const std::string& command) {
  auto args = SplitCommand(command);
  if (args.empty()) return;

  std::string cmd = args[0];
  std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

  if (cmd == "ls") {
    CmdLs(args);
  } else if (cmd == "cd") {
    CmdCd(args);
  } else if (cmd == "cat") {
    CmdCat(args);
  } else if (cmd == "echo") {
    CmdEcho(args);
  } else if (cmd == "help") {
    CmdHelp(args);
  } else if (cmd == "clear") {
    CmdClear(args);
  } else if (cmd == "pwd") {
    CmdPwd(args);
  } else if (cmd == "set") {
    CmdSet(args);
  } else {
    AddOutput("bash: " + cmd + ": command not found");
  }
}

void Terminal::AddOutput(const std::string& text) {
  output_history_.push_back(text);

  // Auto-scroll to bottom when new content is added
  float total_height = (output_history_.size() + 1) * 20.0f;  // +1 for current input line
  float visible_height = GetScreenHeight() * 0.7f;            // Content area height
  if (total_height > visible_height) {
    scroll_offset_ = total_height - visible_height + 20.0f;  // Extra padding to show current line
  }
}

void Terminal::InitializeFilesystem() {
  // Create virtual filesystem structure
  virtual_filesystem_["/"] = {"home", "usr", "var", "readme.txt"};
  virtual_filesystem_["/home"] = {"player", "guest"};
  virtual_filesystem_["/home/player"] = {"documents", "saves", "config.cfg"};
  virtual_filesystem_["/home/player/documents"] = {"notes.txt", "todo.txt"};
  virtual_filesystem_["/usr"] = {"bin", "lib"};
  virtual_filesystem_["/usr/bin"] = {"game", "editor"};
  virtual_filesystem_["/var"] = {"log", "tmp"};
  virtual_filesystem_["/var/log"] = {"game.log", "system.log"};

  // Create some virtual files
  virtual_files_["/readme.txt"] =
      "Welcome to the game terminal!\nThis is a virtual filesystem for demonstration.";
  virtual_files_["/home/player/config.cfg"] =
      "# Game Configuration\nresolution=1920x1080\nfullscreen=false\nvolume=0.8";
  virtual_files_["/home/player/documents/notes.txt"] =
      "Remember to check the secret area behind the waterfall!";
  virtual_files_["/home/player/documents/todo.txt"] =
      "- Complete level 3\n- Find all collectibles\n- Defeat the boss";
  virtual_files_["/var/log/game.log"] =
      "[INFO] Game started\n[DEBUG] Loading assets...\n[INFO] Player entered level 1";
}

void Terminal::CmdLs(const std::vector<std::string>& args) {
  std::string target_path = current_directory_;
  if (args.size() > 1) {
    target_path = ResolvePath(args[1]);
  }

  if (!IsDirectory(target_path)) {
    AddOutput("ls: cannot access '" + target_path + "': No such file or directory");
    return;
  }

  auto it = virtual_filesystem_.find(target_path);
  if (it != virtual_filesystem_.end()) {
    for (const auto& item : it->second) {
      std::string item_path = JoinPath(target_path, item);
      if (IsDirectory(item_path)) {
        AddOutput(item + "/");
      } else {
        AddOutput(item);
      }
    }
  }
}

void Terminal::CmdCd(const std::vector<std::string>& args) {
  if (args.size() < 2) {
    current_directory_ = "/home/player";  // Default home
    return;
  }

  std::string new_path = ResolvePath(args[1]);

  if (!IsDirectory(new_path)) {
    AddOutput("bash: cd: " + args[1] + ": No such file or directory");
    return;
  }

  current_directory_ = new_path;
}

void Terminal::CmdCat(const std::vector<std::string>& args) {
  if (args.size() < 2) {
    AddOutput("cat: missing file operand");
    AddOutput("Try 'cat --help' for more information.");
    return;
  }

  std::string file_path = ResolvePath(args[1]);

  auto it = virtual_files_.find(file_path);
  if (it != virtual_files_.end()) {
    std::stringstream ss(it->second);
    std::string line;
    while (std::getline(ss, line)) {
      AddOutput(line);
    }
  } else {
    AddOutput("cat: " + args[1] + ": No such file or directory");
  }
}

void Terminal::CmdEcho(const std::vector<std::string>& args) {
  std::string output;
  for (size_t i = 1; i < args.size(); ++i) {
    if (i > 1) output += " ";
    output += args[i];
  }
  AddOutput(output);
}

void Terminal::CmdHelp(const std::vector<std::string>& args) {
  AddOutput("Available commands:");
  AddOutput("  ls [directory]     - List directory contents");
  AddOutput("  cd [directory]     - Change directory");
  AddOutput("  cat <file>         - Display file contents");
  AddOutput("  echo <text>        - Display text");
  AddOutput("  pwd                - Print working directory");
  AddOutput("  clear              - Clear terminal");
  AddOutput("  set <prop> <val>   - Change terminal settings");
  AddOutput("  help               - Show this help");
  AddOutput("");
  AddOutput("Use \\ key to toggle terminal");
}

void Terminal::CmdClear(const std::vector<std::string>& args) {
  output_history_.clear();
  scroll_offset_ = 0.0f;
}

void Terminal::CmdPwd(const std::vector<std::string>& args) { AddOutput(current_directory_); }
void Terminal::CmdSet(const std::vector<std::string>& args) {
  // clang-format off
  if (args.size() < 3) {
    AddOutput("Usage: set <property> <value>");
    AddOutput("Available properties:");
    AddOutput("  fontsize <number>     - Set font size (8-32)");
    AddOutput("  bgcolor <color>       - Set background color (hex color or black, dark, gray, blue, green)");
    return;
  }
  // clang-format on

  std::string property = args[1];
  std::string value = args[2];

  // Convert to lowercase for case-insensitive comparison
  std::transform(property.begin(), property.end(), property.begin(), ::tolower);
  std::transform(value.begin(), value.end(), value.begin(), ::tolower);
  if (property == "fontsize") {
    try {
      float new_size = std::stof(value);
      if (new_size >= 8.0f && new_size <= 32.0f) {
        font_size_ = new_size;
        AddOutput("Font size set to " + value);
      } else {
        AddOutput("Error: Font size must be between 8 and 32");
      }
    } catch (const std::exception& e) {
      AddOutput("Error: Invalid font size value");
    }
  } else if (property == "bgcolor") {
    Color color = background_color_;  // Default fallback

    if (value == "dark")
      color = (Color){20, 20, 20, 240};
    else if (value == "black")
      color = (Color){0, 0, 0, 240};
    else if (value == "gray")
      color = (Color){40, 40, 40, 240};
    else if (value == "blue")
      color = (Color){0, 0, 40, 240};
    else if (value == "green")
      color = (Color){0, 40, 0, 240};
    else if (value[0] == '#') {
      color = Hexc(value);
    } else {
      AddOutput("Error: Unknown background color '" + value + "'");
      AddOutput("Available colors: dark, black, gray, darkblue, darkgreen");
      return;
    }

    background_color_ = color;
    AddOutput("Background color set to " + value);
  } else {
    AddOutput("Error: Unknown property '" + args[1] + "'");
    AddOutput("Type 'set' without arguments to see available properties");
  }
}

std::vector<std::string> Terminal::SplitCommand(const std::string& command) {
  std::vector<std::string> result;
  std::stringstream ss(command);
  std::string item;

  while (ss >> item) {
    result.push_back(item);
  }

  return result;
}

std::string Terminal::JoinPath(const std::string& base, const std::string& path) {
  if (base == "/") {
    return "/" + path;
  }
  return base + "/" + path;
}

std::string Terminal::ResolvePath(const std::string& path) {
  if (path.empty()) return current_directory_;

  if (path[0] == '/') {
    return path;  // Absolute path
  }

  if (path == "..") {
    if (current_directory_ == "/") return "/";
    size_t pos = current_directory_.find_last_of('/');
    if (pos == 0) return "/";
    return current_directory_.substr(0, pos);
  }

  if (path == ".") {
    return current_directory_;
  }

  // Relative path
  return JoinPath(current_directory_, path);
}

bool Terminal::PathExists(const std::string& path) {
  return virtual_filesystem_.find(path) != virtual_filesystem_.end() ||
         virtual_files_.find(path) != virtual_files_.end();
}

bool Terminal::IsDirectory(const std::string& path) {
  return virtual_filesystem_.find(path) != virtual_filesystem_.end();
}

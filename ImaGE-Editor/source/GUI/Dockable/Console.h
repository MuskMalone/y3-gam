#pragma once
#include <GUI/GUIWindow.h>
#include <External/ImTerm/terminal.hpp>
#include <External/ImTerm/terminal_helpers.hpp>

namespace GUI {

  struct TerminalSettings { /* empty for now */ };
  class GUITerminal : public ImTerm::basic_spdlog_terminal_helper<GUITerminal, TerminalSettings, misc::no_mutex> {};

  class Console : public GUIWindow {
  public:
    Console(const char* name);

    void Run() override;
    void SetMaxLength(size_t length);

  private:
    ImTerm::terminal<GUITerminal> mTerminal;
    TerminalSettings mSettings;
  };

} // namespace GUI

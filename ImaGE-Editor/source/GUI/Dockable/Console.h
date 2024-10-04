/*!*********************************************************************
\file   Console.h
\author chengen.lau\@digipen.edu
\date   5-October-2024
\brief  Class encapsulating functions to run the console / terminal of
        the editor. Anything sent to the logger will be output to the
        console.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <GUI/GUIWindow.h>
#include <ImTerm/terminal.hpp>
#include <ImTerm/terminal_helpers.hpp>

namespace GUI {

  struct TerminalSettings { /* empty for now */ };
  class GUITerminal : public ImTerm::basic_spdlog_terminal_helper<GUITerminal, TerminalSettings, misc::no_mutex> {};

  class Console : public GUIWindow {
  public:
    Console(const char* name);

    /*!*********************************************************************
    \brief
      Runs the console
    ************************************************************************/
    void Run() override;

    /*!*********************************************************************
    \brief
      Sets a new maximum line display in the console 
    \param length
      The new length to set as the maximum
    ************************************************************************/
    void SetMaxLength(size_t length);

  private:
    ImTerm::terminal<GUITerminal> mTerminal;
    TerminalSettings mSettings;
  };

} // namespace GUI

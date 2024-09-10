#include <pch.h>
#include "FrameRateController.h"
#include <GLFW/glfw3.h>

FRC::TimeType FRC::m_prevFrameTime{}, FRC::m_currFrameTime{},
  FRC::m_deltaTime{}, FRC::m_fpsTimer{}, FRC::m_currFPS{};
unsigned FRC::m_framesElapsed{};

void FRC::Update()
{
  m_prevFrameTime = m_currFrameTime;
  m_currFrameTime = static_cast<TimeType>(glfwGetTime());
  m_deltaTime = m_currFrameTime - m_prevFrameTime;

  m_fpsTimer += m_deltaTime;
  if (m_fpsTimer >= FPSCalcInterval)
  {
    m_currFPS = static_cast<TimeType>(m_framesElapsed) / FPSCalcInterval;
    m_fpsTimer = 0.0;
  }

  ++m_framesElapsed;
}

FRC::TimeType FRC::GetDeltaTime() noexcept
{
  return m_deltaTime;
}

FRC::TimeType FRC::GetFPS() noexcept
{
  return m_currFPS;
}

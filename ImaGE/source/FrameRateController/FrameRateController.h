#pragma once

// Frame Rate Controller
class FRC
{
public:
  using TimeType = float;

  static void Update();
  static TimeType GetDeltaTime() noexcept;
  static TimeType GetFPS() noexcept;

private:
  static TimeType constexpr FPSCalcInterval = 1.0;

  static TimeType m_prevFrameTime, m_currFrameTime, m_deltaTime;
  static TimeType m_fpsTimer, m_currFPS;
  static unsigned m_framesElapsed;
};

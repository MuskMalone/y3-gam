#include <pch.h>
#include "FrameRateController.h"
#include <GLFW/glfw3.h>

void FrameRateController::Init(float targetFPS, float fpsCalculationInterval, bool vsyncEnabled) {
  m_vsyncEnabled = vsyncEnabled;
  SetVsync(m_vsyncEnabled);

  m_currFrameTime = 0.f, m_newFrameTime = 0.f, m_deltaTime = 0.f;
  m_fpsTimer = 0.f, m_currFPS = 0.f;

  m_targetFPS = targetFPS;
  m_targetFrameTime = 1.f / m_targetFPS;
  m_fpsCalculationInterval = fpsCalculationInterval;

  m_frameCounter = 0.f;
}

void FrameRateController::Start() {
  m_deltaTime = m_newFrameTime - m_currFrameTime;
  m_currFrameTime = m_newFrameTime;

  m_fpsTimer += m_deltaTime;
  if (m_fpsTimer >= m_fpsCalculationInterval) {
    m_currFPS = static_cast<TimeType>(m_frameCounter) / m_fpsCalculationInterval;
    m_fpsTimer = 0.f;
  }
}

void FrameRateController::End() {
  do {
    m_newFrameTime = static_cast<TimeType>(glfwGetTime());
    m_deltaTime = m_newFrameTime - m_currFrameTime;
  } while (m_deltaTime < m_targetFrameTime);

  m_newFrameTime = static_cast<TimeType>(glfwGetTime());
  ++m_frameCounter;
}

FrameRateController::TimeType FrameRateController::GetDeltaTime() const noexcept {
  return m_deltaTime;
}

FrameRateController::TimeType FrameRateController::GetFPS() const noexcept {
  return m_currFPS;
}

bool FrameRateController::GetVsyncFlag() const noexcept {
  return m_vsyncEnabled;
}

void FrameRateController::SetVsync(bool vsyncEnabled) {
  m_vsyncEnabled = vsyncEnabled;
  glfwSwapInterval((m_vsyncEnabled) ? 1 : 0);
}

void FrameRateController::Reset() {
  m_currFrameTime = 0.f;
  m_newFrameTime = 0.f;
  m_deltaTime = 0.f;
  m_fpsTimer = 0.f, m_currFPS = 0.f;
  m_frameCounter = 0.f;
}
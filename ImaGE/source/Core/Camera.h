#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <Graphics/ShaderProgram.h>

class Camera
{
public:
  Camera(int width, int height,
    glm::vec3 const& eye, glm::vec3 const& target = { 0.f, 0.f ,0.f }, glm::vec3 const& up = { 0.f, 1.f, 0.f },
    glm::vec3 const& right = { 1.f, 0.f, 0.f }, bool canMove = true, float FOV = glm::quarter_pi<float>() * 1.5f, float near = 0.5f, float far = 100.f);

  inline glm::mat4 const& GetViewMatrix() const noexcept { return m_viewMtx; }
  inline glm::mat4 const& GetProjMatrix() const noexcept { return m_projMtx; }

  void InitControls();
  void Update(float deltaTime);
  void Reset(glm::vec3 const& eye, glm::vec3 const& target);

  void SetUniforms(Graphics::ShaderProgram& shader) const;
  void onCursor(double xOffSet, double yOffSet);

  // Controls
  void Left();
  void Right();
  void Forward();
  void Backward();
  void MoveAlongPlane(float xDisplacement, float yDisplacement);

//private: //tch: i removed it
  static constexpr float MOVEMENT_SPD = 15.f;
  static constexpr float PANNING_SPD = 25.f;

  glm::mat4 m_viewMtx, m_projMtx;
  glm::vec3 m_eye, m_target;
  glm::vec3 const m_defaultUp;
  glm::vec3 m_up, m_right, m_front;
  glm::vec3 m_rotation, m_rotationDelta; // pitch, yaw, roll
  glm::vec3 m_movementDelta, m_sideMovementDelta;
  float m_aspectRatio, m_fieldOfView;
  float m_near, m_far;
  bool m_modified;
  bool const m_canMove;
  
  void ComputeAxes();
};

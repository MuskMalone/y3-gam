#include <pch.h>
#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(int width, int height,
  glm::vec3 const& eye, glm::vec3 const& target, glm::vec3 const& up, glm::vec3 const& right, bool canMove,
  float FOV, float near, float far)
    : m_viewMtx{}, m_projMtx{}, m_eye { eye }, m_target{ target }, m_defaultUp{ up }, m_up{ up }, m_right{ right }, m_front{},
    m_rotation{ 0.f, -90.f, 0.f }, m_rotationDelta{}, m_movementDelta{}, m_aspectRatio{ static_cast<float>(width) / static_cast<float>(height) },
    m_canMove{ canMove }, m_fieldOfView{ FOV }, m_near{ near }, m_far{ far }, m_modified{ canMove }
{
  m_viewMtx = glm::mat4(1.0f);
  m_viewMtx = glm::lookAt(m_eye, m_target, m_up);

  m_projMtx = glm::mat4(1.0f);
  m_projMtx = glm::perspective(m_fieldOfView, m_aspectRatio, m_near, m_far);
}

void Camera::InitControls()
{
 
}

void Camera::Update(float deltaTime)
{
  // if untouched, don't need to recompute
  //if (!m_modified) { return; }

  m_eye += MOVEMENT_SPD * m_movementDelta * deltaTime;

  m_eye += MOVEMENT_SPD * m_sideMovementDelta * deltaTime;
  m_target += MOVEMENT_SPD * m_sideMovementDelta * deltaTime;
  
  m_rotation += PANNING_SPD * m_rotationDelta * deltaTime;
  m_rotation.x = glm::clamp(m_rotation.x, -89.f, 89.f);
  m_rotation.y = glm::mod(m_rotation.y, 360.f);
  ComputeAxes();
  
  m_viewMtx = glm::mat4(1.0f);
  m_viewMtx = glm::lookAt(m_eye, m_target, m_up);

  m_movementDelta = m_rotationDelta = m_sideMovementDelta = {};
  m_modified = false;
}


void Camera::onCursor(double xOffSet, double yOffSet)
{
  m_rotationDelta.x = -static_cast<float>(yOffSet) * PANNING_SPD;
  m_rotationDelta.y = static_cast<float>(xOffSet) * PANNING_SPD;

  m_modified = true;
}

void Camera::ComputeAxes()
{
  m_front = {
    cos(glm::radians(m_rotation.y)) * cos(glm::radians(m_rotation.x)),
    sin(glm::radians(m_rotation.x)),
    sin(glm::radians(m_rotation.y)) * cos(glm::radians(m_rotation.x))
  };
  m_front = glm::normalize(m_front);

  m_right = glm::normalize(glm::cross(m_front, m_defaultUp));
  m_up = glm::normalize(glm::cross(m_right, m_front));
  m_target = m_eye + m_front;
}

void Camera::Reset(glm::vec3 const& eye, glm::vec3 const& target)
{
  m_eye = eye;
  m_target = target;
  m_up = m_defaultUp;
  m_right = { 1.f, 0.f, 0.f };
  m_rotation = { 0.f, -90.f, 0.f };
  m_modified = true;
}

void Camera::Left()
{
  m_movementDelta = -m_right * MOVEMENT_SPD;
  m_modified = true;
}

void Camera::Right()
{
  m_movementDelta = m_right * MOVEMENT_SPD;
  m_modified = true;
}

void Camera::Forward()
{
  m_movementDelta = m_front * MOVEMENT_SPD;
  m_modified = true;
}

void Camera::Backward()
{
  m_movementDelta = -m_front * MOVEMENT_SPD;
  m_modified = true;
}

void Camera::MoveAlongPlane(float xDisplacement, float yDisplacement) {
    // Calculate the normal of the plane (eye to target direction)
    glm::vec3 normal = glm::normalize(m_target - m_eye);

    // Define the world up vector
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    // Calculate the right and up vectors on the plane
    glm::vec3 right = glm::normalize(glm::cross(up, normal));  // Right vector
    glm::vec3 planeUp = glm::normalize(glm::cross(normal, right)); // True up vector on the plane

    // Calculate the movement vector along the plane
    m_sideMovementDelta = right * xDisplacement + planeUp * yDisplacement;

    //// Move the eye and target along the plane
    //m_eye += movement;
    //m_target += movement;
}

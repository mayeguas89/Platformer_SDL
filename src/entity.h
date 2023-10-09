#pragma once

#include <glm/glm.hpp>

// clang-format off
#define SDL_MAIN_HANDLED
#include <SDL.h>
// clang-format on

#include "texture_map.h"
#include "utils/state_machine.h"

#include <memory>
#include <string>
#include <tuple>

class Entity
{
public:
  Entity(const std::string& animationFile = "", const std::optional<State>& state = std::nullopt);
  ~Entity() = default;

  // // Simple move constructor
  // Entity(Entity&& other):
  //   texture_{std::move(other.texture_)},
  //   position_{other.position_},
  //   rotation_{other.rotation_},
  //   scale_{other.scale_}
  // {}

  // // Simple move assignment operator
  // Entity& operator=(Entity&& other)
  // {
  //   texture_ = std::move(other.texture_);
  //   position_ = other.position_;
  //   rotation_ = other.rotation_;
  //   scale_ = other.scale_;
  //   return *this;
  // }

  enum class LookAt
  {
    Right,
    Left
  };

  std::string GetState() const
  {
    return state_;
  }

  glm::vec4 GetPosition() const
  {
    return position_;
  }

  const glm::vec4& GetRotation() const
  {
    return rotation_;
  }

  const glm::vec4& GetScale() const
  {
    return scale_;
  }

  void SetPosition(const glm::vec4& position)
  {
    position_ = position;
  }

  void SetRotation(const glm::vec4& rotation)
  {
    rotation_ = rotation;
  }

  void SetScale(const glm::vec4& scale)
  {
    scale_ = scale;
  }

  void SetState(const std::string& state)
  {
    state_ = state;
  }

  virtual void Update(const float deltaTime)
  {
    stateMachine_.Update(deltaTime);
  }

  virtual std::pair<SDL_Texture*, SDL_Rect> GetTexture()
  {
    return textureMap_.GetTexture(state_, animationIndex_);
  }

  virtual void ChangeState(const std::string& newState)
  {
    animationIndex_ = 0;
    state_ = newState;
  }

  SDL_RendererFlip GetFlip() const
  {
    if (lookAt_ == LookAt::Right)
      return SDL_FLIP_NONE;
    return SDL_FLIP_HORIZONTAL;
  }

  virtual void Draw();

  glm::vec2 GetCurrentSpeed() const
  {
    return currentSpeed_;
  }

  void SetCurrentSpeed(const glm::vec2& speed)
  {
    currentSpeed_ = speed;
  }

  void SetMaxSpeed(const glm::vec2& maxSpeed)
  {
    maxSpeed_ = maxSpeed;
  }

  glm::vec2 GetMaxSpeed() const
  {
    return maxSpeed_;
  }

  bool IsGrounded() const
  {
    return isGrounded_;
  }

  void SetOnGround(bool value)
  {
    isGrounded_ = value;
  }

  LookAt GetLookAt() const
  {
    return lookAt_;
  }

protected:
  inline static uint32_t id_{0};

  glm::vec2 maxSpeed_{70.f, -150.f};
  glm::vec2 deceleration_{-500.f, 0.f};
  glm::vec2 currentSpeed_{0.f, 0.f};
  bool isGrounded_{true};
  glm::vec4 position_;
  glm::vec4 rotation_;
  glm::vec4 scale_;
  TextureMap textureMap_;
  std::string state_;
  int animationIndex_{0};
  LookAt lookAt_{LookAt::Right};
  StateMachine stateMachine_;
};

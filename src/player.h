#pragma once
#include "entity.h"

#include <fmt/core.h>

#include <map>
#include <string>

class Player: public Entity
{
public:
  Player(const std::string& textureFile): Entity{textureFile}
  {
    CreateStates();
    RegisterMovement();
  }
  ~Player() = default;

  virtual void Update(const float deltaTime) override;

  virtual void ChangeState(const std::string& newState) override
  {
    Entity::ChangeState(newState);
    stateMachine_.ChangeState(statesMap_[fmt::format("player/{}", newState)]);
  }

  virtual void Draw() override;

private:
  void RegisterMovement();
  void CreateStates();

  std::map<std::string, State> statesMap_;
};
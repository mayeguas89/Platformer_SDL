#pragma once

#include "SDL.h"
#include "glm/glm.hpp"
#include "level/level.h"
#include "sdl_input_manager.h"
#include "spdlog/spdlog.h"
#include "utils/singleton.h"

class Camera: public Singleton<Camera>
{
public:
  void SetViewSize(glm::vec<2, int> size)
  {
    rect.w = size.x;
    rect.h = size.y;
    rect.x = 0;
    rect.y = 0;
  }

  void SetPosition(glm::vec<2, int> position)
  {
    rect.x = position.x;
    rect.y = position.y;
  }

  SDL_Rect GetView() const
  {
    return rect;
  }

  void Update(const float deltaTime)
  {
    auto& inputManager = SDLInputManager::Instance();
    auto& level = Level::Instance();
    if (inputManager.IsKeyDown(SDL_SCANCODE_Q))
    {
      rect.w = std::min(rect.w + 1, level.GetMapSize().x - 1);
      rect.h = std::min(rect.h + 1, level.GetMapSize().y - 1);
    }
    if (inputManager.IsKeyDown(SDL_SCANCODE_E))
    {
      rect.w = std::max(rect.w - 1, 5);
      rect.h = std::max(rect.h - 1, 5);
    }
    if (inputManager.IsKeyDown(SDL_SCANCODE_D))
    {
      rect.x = std::min(rect.x + 1, level.GetMapSize().x - rect.w);
    }
    if (inputManager.IsKeyDown(SDL_SCANCODE_A))
    {
      rect.x = std::max(rect.x - 1, 0);
    }
    if (inputManager.IsKeyDown(SDL_SCANCODE_S))
    {
      rect.y = std::min(rect.y + 1, level.GetMapSize().y - rect.h);
    }
    if (inputManager.IsKeyDown(SDL_SCANCODE_W))
    {
      rect.y = std::max(rect.y - 1, 0);
    }

    // spdlog::info("Camera position {},{}", rect.x, rect.y);
  }

private:
  SDL_Rect rect;
};
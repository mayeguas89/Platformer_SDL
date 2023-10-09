#pragma once
#include "entity.h"
#include "level/level.h"
#include "sdl_render.h"
#include "utils/singleton.h"
#include "utils/time_manager.h"

#include <vector>

struct Movement: public Singleton<Movement>
{
  void operator()(std::shared_ptr<Entity> entity);
};

struct Collision: public Singleton<Collision>
{
  void operator()(std::shared_ptr<Entity> entity);
};

struct Gravity: public Singleton<Gravity>
{
  void operator()(std::shared_ptr<Entity> entity);

  inline static float initialGravity{75.f};
  inline static float gravityAcceleration{initialGravity};
  inline static float multiplier{1.1f};
};

class Physics: public Singleton<Physics>
{
public:
  void RegisterEntity(std::shared_ptr<Entity> entity)
  {
    entities_.push_back(std::move(entity));
  }

  void Update(const float deltaTime)
  {
    debugData.clear();

    if (gravity)
    {
      std::for_each(entities_.begin(), entities_.end(), gravity_);
    }

    if (collisions)
    {
      std::for_each(entities_.begin(), entities_.end(), collision_);
    }

    std::for_each(entities_.begin(), entities_.end(), movement_);
  }

  void Draw()
  {
    if (!viewDebugData)
      return;
    SDL_SetRenderDrawColor(SDLRender::Instance().GetRenderer(), 0, 0xFF, 0, 0xFF);
    std::for_each(debugData.begin(),
                  debugData.end(),
                  [](const auto& rect) { SDL_RenderDrawRect(SDLRender::Instance().GetRenderer(), &rect); });
  }

  std::vector<SDL_Rect> debugData;

  bool gravity{true};
  bool collisions{true};
  bool viewDebugData{true};

private:
  Gravity& gravity_{Gravity::Instance()};
  Collision& collision_{Collision::Instance()};
  Movement& movement_{Movement::Instance()};
  std::vector<std::shared_ptr<Entity>> entities_;
};
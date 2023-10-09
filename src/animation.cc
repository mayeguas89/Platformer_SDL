#include "animation.h"

#include "utils/time_manager.h"

#include <algorithm>

void AnimationManager::Update()
{
  float deltaTime = TimeManager::deltaTime;
  std::for_each(animationMap_.begin(),
                animationMap_.end(),
                [&deltaTime](auto& it)
                {
                  auto& [elapsedTime, timeBetweenFrames, function] = it.second;
                  elapsedTime += deltaTime;
                });
  Check();
}

void AnimationManager::RegisterAnimation(const std::string& key,
                                         const float timeBetweenFrames,
                                         std::function<void()> function)
{
  animationMap_.insert({key, {0.f, timeBetweenFrames, function}});
}

void AnimationManager::UnRegisterAnimation(const std::string& key)
{
  animationMap_.erase(key);
}

void AnimationManager::Check()
{
  for (auto it = animationMap_.begin(); it != animationMap_.end(); it++)
  {
    auto& [elapsedTime, timeBetweenFrames, function] = it->second;
    if (elapsedTime > timeBetweenFrames)
    {
      elapsedTime = std::min(elapsedTime - timeBetweenFrames, 0.f);
      function();
    }
  }
}

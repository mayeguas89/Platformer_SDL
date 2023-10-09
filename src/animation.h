#pragma once

#include "utils/singleton.h"

#include <functional>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

struct Animation
{
  Animation(const float p_timeBetweenFrames, const bool p_loop, const int p_row = -1, const int p_numCols = -1):
    timeBetweenFrames{p_timeBetweenFrames},
    loop{p_loop},
    row{p_row},
    numCols{p_numCols}
  {}
  float timeBetweenFrames;
  bool loop;
  int row{-1};
  int numCols{-1};
};

class AnimationManager: public Singleton<AnimationManager>
{
public:
  AnimationManager() = default;
  ~AnimationManager() = default;
  void Update();
  void RegisterAnimation(const std::string& key, const float timeBetweenFrames, std::function<void()> function);
  void UnRegisterAnimation(const std::string& key);

private:
  void Check();
  std::unordered_map<std::string, std::tuple<float, float, std::function<void()>>> animationMap_;
};

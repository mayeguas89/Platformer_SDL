#pragma once

#include <chrono>

class TimeManager
{
public:
  inline static float deltaTime{0.f};
  inline static std::chrono::time_point<std::chrono::high_resolution_clock> newTime;
  inline static std::chrono::time_point<std::chrono::high_resolution_clock> oldTime;

  static void Update();
};
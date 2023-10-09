#include "time_manager.h"

void TimeManager::Update()
{
  static bool firstTime{true};
  if (firstTime)
  {
    oldTime = std::chrono::high_resolution_clock::now();
    firstTime = false;
  }

  auto timeNow = std::chrono::high_resolution_clock::now();
  auto timeElapsed = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(timeNow - oldTime);
  deltaTime = timeElapsed.count();
  oldTime = timeNow;
}
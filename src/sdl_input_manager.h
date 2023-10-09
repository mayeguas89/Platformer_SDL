#pragma once

#include "utils/singleton.h"

#include <SDL.h>
#include <functional>
#include <map>

class SDLInputManager: public Singleton<SDLInputManager>
{
public:
  SDLInputManager()
  {
    for (int i = 0; i < SDL_NUM_SCANCODES; i++)
      keyMap_[static_cast<SDL_Scancode>(i)] = false;
  }
  ~SDLInputManager() = default;

  void PollEvents();

  void RegisterQuitFunction(std::function<void(void)> function);
  void RegisterKeyDownFunction(SDL_Scancode key, std::function<void(void)> function);
  void RegisterKeyUpFunction(SDL_Scancode key, std::function<void(void)> function);

  bool IsKeyDown(SDL_Scancode key)
  {
    return keyMap_[key];
  }

private:
  std::function<void(void)> quitFunction_;
  std::map<SDL_Scancode, std::function<void(void)>> keyDownMapFunction_;
  std::map<SDL_Scancode, std::function<void(void)>> keyUpMapFunction_;

  std::unordered_map<SDL_Scancode, bool> keyMap_;
};

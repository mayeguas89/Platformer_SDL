#include "sdl_input_manager.h"

// clang-format off
#define SDL_MAIN_HANDLED
#include <SDL.h>
// clang-format on

#include "imgui_impl_sdl2.h"

void SDLInputManager::PollEvents()
{
  SDL_Event event;

  while (SDL_PollEvent(&event))
  {
    ImGui_ImplSDL2_ProcessEvent(&event);
    switch (event.type)
    {
      case SDL_QUIT:
        quitFunction_();
        break;
      case SDL_KEYDOWN:
      {
        if (event.key.repeat == 0)
        {
          if (auto it = keyDownMapFunction_.find(event.key.keysym.scancode); it != keyDownMapFunction_.end())
          {
            it->second();
          }
        }
        keyMap_[event.key.keysym.scancode] = true;

        break;
      }
      case SDL_KEYUP:
      {
        if (event.key.repeat == 0)
        {
          if (auto it = keyUpMapFunction_.find(event.key.keysym.scancode); it != keyUpMapFunction_.end())
          {
            it->second();
          }
        }
        keyMap_[event.key.keysym.scancode] = false;
        break;
      }

      default:
        break;
    }
  }
}

void SDLInputManager::RegisterQuitFunction(std::function<void(void)> function)
{
  quitFunction_ = function;
}

void SDLInputManager::RegisterKeyDownFunction(SDL_Scancode key, std::function<void(void)> function)
{
  keyDownMapFunction_[key] = function;
}

void SDLInputManager::RegisterKeyUpFunction(SDL_Scancode key, std::function<void(void)> function)
{
  keyUpMapFunction_[key] = function;
}

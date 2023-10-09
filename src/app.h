#pragma once

#include "entity.h"
#include "sdl_input_manager.h"
#include "sdl_render.h"
#include "utils/singleton.h"

#include <string>
#include <vector>

class App: public Singleton<App>
{
public:
  App(const std::string& appName, const int width, const int height);
  ~App();

  void Init();
  void Run();
  void End();

private:
  std::string name_;
  int width_;
  int height_;

  bool isRunning_{true};

  SDLRender& render_;
  SDLInputManager& inputManager_;
  std::vector<std::shared_ptr<Entity>> objects_;
};

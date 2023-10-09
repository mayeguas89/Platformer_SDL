#pragma once

// clang-format off
#define SDL_MAIN_HANDLED
#include <SDL.h>
// clang-format on

#include "entity.h"
#include "fmt/core.h"
#include "ui/imgui_render.h"
#include "utils/singleton.h"

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <vector>

class SDLRender: public Singleton<SDLRender>
{
public:
  SDLRender();
  ~SDLRender();
  void Init(const std::string& title, int width, int height);
  void StartFrame();
  void EndFrame();
  void DrawScene(const std::vector<std::shared_ptr<Entity>>& objects);
  void ClearScene();
  void SetClearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
  void Destroy();
  void DrawObject(const std::shared_ptr<Entity>& object);
  void Render();

  int GetWidth() const
  {
    return width_;
  }

  int GetHeight() const
  {
    return height_;
  }

  SDL_Renderer* GetRenderer() const
  {
    return renderer_.get();
  }

private:
  struct SDLRendererDestroyer
  {
    void operator()(SDL_Renderer* renderer) const
    {
      SDL_DestroyRenderer(renderer);
    }
  };
  struct SDLWindowDestroyer
  {
    void operator()(SDL_Window* w) const
    {
      SDL_DestroyWindow(w);
    }
  };

  void CreateWindow(const std::string& title, int width, int height);
  void CreateRenderer();
  std::unique_ptr<SDL_Window, SDLWindowDestroyer> window_;
  std::unique_ptr<SDL_Renderer, SDLRendererDestroyer> renderer_;
  SDL_Color clearColor_{96, 128, 255, 255};

  ImGuiRender& imguiRender_;

  int width_, height_;
};

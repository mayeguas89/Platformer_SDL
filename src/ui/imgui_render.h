#pragma once

#include "entity.h"
#include "imgui.h"
#include "utils/singleton.h"

#include <SDL.h>
#include <memory>
#include <string>
#include <unordered_map>


class ImGuiRender: public Singleton<ImGuiRender>
{
public:
  ImGuiRender() = default;
  ~ImGuiRender() = default;
  void Init(SDL_Window* window, SDL_Renderer* renderer);
  void StartFrame();
  void Render();
  void End();

  void SetClearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

  void RegisterEntity(const std::string& name, std::shared_ptr<Entity> entity);

  SDL_Color GetClearColor();

private:
  void EntitiesWindow();
  void PhysicsWindow();
  ImVec4 clearColor_ = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  std::unordered_map<std::string, std::shared_ptr<Entity>> entities_;
};

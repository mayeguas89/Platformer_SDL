#include "app.h"

#include "animation.h"
#include "background.h"
#include "camera.h"
#include "level/level.h"
#include "physics.h"
#include "player.h"
#include "utils/helpers.h"
#include "utils/time_manager.h"

App::App(const std::string& appName, const int width, const int height):
  name_{appName},
  width_{width},
  height_{height},
  render_{SDLRender::Instance()},
  inputManager_{SDLInputManager::Instance()}
{}

App::~App() {}

void App::Init()
{
  render_.Init(name_, width_, height_);
  inputManager_.RegisterQuitFunction([this]() { isRunning_ = false; });

  std::shared_ptr<Entity> background = std::make_shared<Background>("Assets/Background/texture_map.json");
  // objects_.push_back(background);

  std::shared_ptr<Entity> player = std::make_shared<Player>("Assets/Characters/texture_map.json");
  player->SetPosition({100.f, 100.f, 0.f, 0.f});
  player->SetState("idle");
  objects_.push_back(player);

  auto& level = Level::Instance();

  utils::ReadLevelFile("Assets/main.tmj", level);

  Physics::Instance().RegisterEntity(player);
  ImGuiRender::Instance().RegisterEntity("player", player);

  auto& camera = Camera::Instance();
  const int numTilesX = 20;
  const int numTilesY = 20;
  camera.SetViewSize({numTilesX, numTilesY});
  camera.SetPosition({0, level.GetMapSize().y - numTilesY});
}

void App::Run()
{
  while (isRunning_)
  {
    inputManager_.PollEvents();

    render_.ClearScene();

    // Update
    TimeManager::Update();
    Physics::Instance().Update(TimeManager::deltaTime);
    AnimationManager::Instance().Update();
    Camera::Instance().Update(TimeManager::deltaTime);
    std::for_each(objects_.begin(),
                  objects_.end(),
                  [](std::shared_ptr<Entity> object) { object->Update(TimeManager::deltaTime); });
    // Render
    render_.StartFrame();
    Level::Instance().Draw();
    render_.DrawScene(objects_);
    Physics::Instance().Draw();
    render_.Render();
    render_.EndFrame();
  }
}

void App::End()
{
  render_.Destroy();
}
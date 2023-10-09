#include "player.h"

#include "fmt/core.h"
#include "sdl_input_manager.h"
#include "sdl_render.h"

#include <spdlog/spdlog.h>

void Player::RegisterMovement()
{
  auto& inputManager = SDLInputManager::Instance();

  // inputManager.RegisterKeyDownFunction(SDL_SCANCODE_DOWN, [this]() { moveDown_ = true; });
  // inputManager.RegisterKeyUpFunction(SDL_SCANCODE_DOWN, [this]() { moveDown_ = false; });

  // inputManager.RegisterKeyDownFunction(SDL_SCANCODE_UP, [this]() { moveUp_ = true; });
  // inputManager.RegisterKeyUpFunction(SDL_SCANCODE_UP, [this]() { moveUp_ = false; });

  // inputManager.RegisterKeyDownFunction(SDL_SCANCODE_LEFT,
  //                                      [this]()
  //                                      {
  //                                        moveLeft_ = true;
  //                                        lookAt_ = LookAt::Left;
  //                                        ChangeState("run");
  //                                      });
  // inputManager.RegisterKeyUpFunction(SDL_SCANCODE_LEFT,
  //                                    [this]()
  //                                    {
  //                                      moveLeft_ = false;
  //                                      ChangeState("idle");
  //                                    });

  // inputManager.RegisterKeyDownFunction(SDL_SCANCODE_RIGHT,
  //                                      [this, &inputManager]()
  //                                      {
  //                                        movingXDir_ = true;
  //                                        lookAt_ = LookAt::Right;
  //                                        if (!inputManager.IsKeyDown(SDL_SCANCODE_LEFT))
  //                                          ChangeState("run");
  //                                      });
  // inputManager.RegisterKeyUpFunction(SDL_SCANCODE_RIGHT,
  //                                    [this, &inputManager]()
  //                                    {
  //                                      movingXDir_ = false;
  //                                      if (!inputManager.IsKeyDown(SDL_SCANCODE_LEFT))
  //                                        ChangeState("idle");
  //                                    });

  inputManager.RegisterKeyDownFunction(SDL_SCANCODE_SPACE, [this]() { ChangeState("attack"); });
}

void Player::CreateStates()
{
  State playerIdle;
  playerIdle.RegisterEnterCallback(
    [this]()
    {
      AnimationManager::Instance().RegisterAnimation("player/idle",
                                                     textureMap_.GetTimeBetweenFrames("idle"),
                                                     [this]()
                                                     {
                                                       auto numberOfTextures =
                                                         textureMap_.GetNumberOfTextures("idle");
                                                       animationIndex_ = (animationIndex_ + 1) % numberOfTextures;
                                                     });
    });
  playerIdle.RegisterOnStateCallback(
    [this](const float deltaTime)
    {
      auto& inputManager = SDLInputManager::Instance();
      const float seconds = deltaTime / 1000.f;

      if (std::abs(currentSpeed_.x) > 0.f)
      {
        if (lookAt_ == LookAt::Left)
        {
          currentSpeed_.x = std::min(currentSpeed_.x - deceleration_.x * seconds, 0.f);
        }
        if (lookAt_ == LookAt::Right)
        {
          currentSpeed_.x = std::max(currentSpeed_.x + deceleration_.x * seconds, 0.f);
        }
      }

      if (inputManager.IsKeyDown(SDL_SCANCODE_UP) && isGrounded_)
      {
        currentSpeed_.y = maxSpeed_.y;
        isGrounded_ = false;
      }

      if (inputManager.IsKeyDown(SDL_SCANCODE_LEFT) || inputManager.IsKeyDown(SDL_SCANCODE_RIGHT))
        ChangeState("run");
    });
  playerIdle.RegisterExitCallback([]() { AnimationManager::Instance().UnRegisterAnimation("player/idle"); });
  statesMap_["player/idle"] = playerIdle;

  State playerRun;
  playerRun.RegisterEnterCallback(
    [this]()
    {
      AnimationManager::Instance().RegisterAnimation("player/run",
                                                     textureMap_.GetTimeBetweenFrames("run"),
                                                     [this]()
                                                     {
                                                       auto numberOfTextures =
                                                         textureMap_.GetNumberOfTextures("run");
                                                       animationIndex_ = (animationIndex_ + 1) % numberOfTextures;
                                                     });
    });
  playerRun.RegisterExitCallback([]() { AnimationManager::Instance().UnRegisterAnimation("player/run"); });
  playerRun.RegisterOnStateCallback(
    [this](const float deltaTime)
    {
      auto& inputManager = SDLInputManager::Instance();
      const float seconds = deltaTime / 1000.f;
      // currentSpeed_.x = std::min(currentSpeed_.x + acceleration_.x * seconds, maxSpeed_.x);

      if (inputManager.IsKeyDown(SDL_SCANCODE_LEFT))
      {
        lookAt_ = LookAt::Left;
        // currentSpeed_.x = std::min(currentSpeed_.x + acceleration_.x * seconds, maxSpeed_.x);
        currentSpeed_.x = -maxSpeed_.x;
      }
      if (inputManager.IsKeyDown(SDL_SCANCODE_RIGHT))
      {
        lookAt_ = LookAt::Right;
        // position_.x += currentSpeed_.x * seconds;
        currentSpeed_.x = maxSpeed_.x;
      }

      if (inputManager.IsKeyDown(SDL_SCANCODE_UP) && isGrounded_)
      {
        currentSpeed_.y = maxSpeed_.y;
        isGrounded_ = false;
      }

      if (!inputManager.IsKeyDown(SDL_SCANCODE_RIGHT) && !inputManager.IsKeyDown(SDL_SCANCODE_LEFT))
        ChangeState("idle");
    });
  statesMap_["player/run"] = playerRun;

  State playerAttack;
  playerAttack.RegisterEnterCallback(
    [this]()
    {
      currentSpeed_.x = 0.f;
      AnimationManager::Instance().RegisterAnimation("player/attack",
                                                     textureMap_.GetTimeBetweenFrames("attack"),
                                                     [this]()
                                                     {
                                                       auto numberOfTextures =
                                                         textureMap_.GetNumberOfTextures("attack");
                                                       animationIndex_ = (animationIndex_ + 1);
                                                       if (animationIndex_ > numberOfTextures)
                                                         ChangeState("idle");
                                                     });
    });
  playerAttack.RegisterExitCallback([]() { AnimationManager::Instance().UnRegisterAnimation("player/attack"); });
  statesMap_["player/attack"] = playerAttack;

  stateMachine_.ChangeState(statesMap_["player/idle"]);
}

void Player::Update(const float deltaTime)
{
  Entity::Update(deltaTime);

  // if (isGrounded_)
  // {
  //   currentSpeed_.y = 0.f;
  // }

  // spdlog::info("Player Current Speed Y {}", currentSpeed_.y);
}

void Player::Draw()
{
  SDL_Rect dest;
  const auto& position = GetPosition();
  auto [texture, rect] = GetTexture();
  auto flip = GetFlip();
  dest.x = position.x;
  dest.y = position.y;
  dest.w = rect.w;
  dest.h = rect.h;
  SDL_RenderCopyEx(SDLRender::Instance().GetRenderer(), texture, &rect, &dest, 0.f, nullptr, flip);
}

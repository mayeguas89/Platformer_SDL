#include "sdl_render.h"

#include <SDL_image.h>
#include <SDL_rect.h>

SDLRender::SDLRender():
  window_{std::unique_ptr<SDL_Window, SDLWindowDestroyer>()},
  renderer_{std::unique_ptr<SDL_Renderer, SDLRendererDestroyer>()},
  imguiRender_{ImGuiRender::Instance()}
{
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    throw std::runtime_error(fmt::format("SDL could not initialize! SDL_Error: {}\n", SDL_GetError()));
  }

  IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
}

SDLRender::~SDLRender() {}

void SDLRender::Destroy()
{
  imguiRender_.End();
  window_.release();
  renderer_.release();
}

inline void SDLRender::DrawObject(const std::shared_ptr<Entity>& object)
{
  SDL_Rect dest;
  SDL_Rect* src = nullptr;

  const auto& position = object->GetPosition();
  auto [texture, rect] = object->GetTexture();
  auto flip = object->GetFlip();
  dest.x = position.x;
  dest.y = position.y;
  src = &rect;
  SDL_QueryTexture(texture, nullptr, nullptr, &dest.w, &dest.h);
  dest.w = rect.w;
  dest.h = rect.h;
  SDL_RenderCopyEx(renderer_.get(), texture, src, &dest, 0.f, nullptr, flip);
}

void SDLRender::Render()
{
  // Send drawing commands to the gpu
  SDL_RenderPresent(renderer_.get());
}

void SDLRender::Init(const std::string& title, int width, int height)
{
  CreateWindow(title, width, height);
  CreateRenderer();

  imguiRender_.Init(window_.get(), renderer_.get());
}

void SDLRender::StartFrame()
{
  imguiRender_.StartFrame();
}

void SDLRender::EndFrame()
{
  // Retrieve clear color for clear next frame
  clearColor_ = imguiRender_.GetClearColor();
  // limit our loop to around 62 frames per second
  SDL_Delay(16);
}

void SDLRender::DrawScene(const std::vector<std::shared_ptr<Entity>>& objects)
{
  // imguiRender_.SetClearColor(clearColor_.r, clearColor_.g, clearColor_.b, clearColor_.a);

  // Render ImGui
  imguiRender_.Render();

  // Draw Objects
  std::for_each(objects.begin(), objects.end(), [this](const std::shared_ptr<Entity> object) { object->Draw(); });
}

void SDLRender::ClearScene()
{
  SDL_SetRenderDrawColor(renderer_.get(), clearColor_.r, clearColor_.g, clearColor_.b, clearColor_.a);
  SDL_RenderClear(renderer_.get());
}

void SDLRender::SetClearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
  clearColor_ = {r, g, b, a};
}

void SDLRender::CreateWindow(const std::string& title, int width, int height)
{
  width_ = width;
  height_ = height;
  int windowFlags{0};
  window_.reset(
    SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, windowFlags));
  if (!window_)
    throw std::runtime_error(fmt::format("Fail to open window! SDL_Error: {}\n", SDL_GetError()));
  // SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
}

void SDLRender::CreateRenderer()
{
  int rendererFlags = SDL_RENDERER_ACCELERATED;
  renderer_.reset(SDL_CreateRenderer(window_.get(), -1, rendererFlags));
  if (!renderer_)
    throw std::runtime_error(fmt::format("Fail to create rendered! SDL_Error: {}\n", SDL_GetError()));
}

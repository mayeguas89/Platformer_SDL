#pragma once

#include "entity.h"

class Background: public Entity
{
public:
  Background(const std::string& textureFile): Entity{textureFile} {}
  ~Background() = default;

  virtual void Draw() override
  {
    const auto files = textureMap_.GetFiles();
    const auto& sdlRender = SDLRender::Instance();
    for (const auto& file: files)
    {
      auto [texture, _] = textureMap_.GetTexture(file);
      SDL_Rect dest;
      dest.x = 0;
      dest.y = 0;
      SDL_QueryTexture(texture, nullptr, nullptr, &dest.w, &dest.h);
      dest.w = sdlRender.GetWidth();
      dest.h = sdlRender.GetHeight();
      SDL_RenderCopy(sdlRender.GetRenderer(), texture, nullptr, &dest);
    }
  }
};
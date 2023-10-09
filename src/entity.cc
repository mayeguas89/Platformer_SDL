#include "entity.h"

#include "sdl_render.h"
#include "utils/helpers.h"

Entity::Entity(const std::string& texturesFile, const std::optional<State>& state): stateMachine_{state}
{
  id_++;
  if (!texturesFile.empty())
    utils::ReadTextureMapJsonFile(texturesFile, textureMap_);
}

void Entity::Draw()
{
  SDL_Rect dest;
  SDL_Rect* src = nullptr;

  const auto& position = GetPosition();
  auto [texture, rect] = GetTexture();
  auto flip = GetFlip();
  dest.x = position.x;
  dest.y = position.y;
  src = &rect;
  SDL_QueryTexture(texture, nullptr, nullptr, &dest.w, &dest.h);
  dest.w = rect.w;
  dest.h = rect.h;
  SDL_RenderCopyEx(SDLRender::Instance().GetRenderer(), texture, src, &dest, 0.f, nullptr, flip);
}

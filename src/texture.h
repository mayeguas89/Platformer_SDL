#pragma once

#include <SDL.h>

struct SDLTextureDestroyer
{
  void operator()(SDL_Texture* texture) const
  {
    SDL_DestroyTexture(texture);
  }
};
#include "texture_map.h"

#include "texture.h"
#include "utils/helpers.h"

#include <ranges>

TextureMap::~TextureMap()
{
  // for (auto& it = textureMap_.begin(); it != textureMap_.end(); it++)
  // {

  //   it->second.();
  // }
}

void TextureMap::AddTexture(const std::string& filename)
{
  if (auto it = textureMap_.find(filename); it == textureMap_.end())
  {
    textureMap_.insert({filename, nullptr});
    auto texture = utils::LoadTexture(filename);
    textureMap_[filename] = std::move(std::shared_ptr<SDL_Texture>(texture, SDLTextureDestroyer{}));
  }
}

void TextureMap::AddAnimation(const std::string& animationName,
                              const std::string& filename,
                              const Animation& animation)
{
  if (auto it = textureMap_.find(filename); it == textureMap_.end())
  {
    AddTexture(filename);
  }

  auto it = animationMap_.find(animationName);
  if (it == animationMap_.end())
  {
    animationMap_.insert({animationName, {animation, textureMap_[filename]}});
  }
}

std::pair<SDL_Texture*, SDL_Rect> TextureMap::GetTexture(const std::string& animationName, const int index) const
{
  auto it = animationMap_.find(animationName);
  const auto& [animation, texture] = it->second;
  SDL_Rect rect;
  rect.w = frameWidth;
  rect.h = frameHeight;
  rect.x = index * frameWidth;
  rect.y = animation.row * frameHeight;
  return {texture.get(), rect};
}

std::pair<SDL_Texture*, SDL_Rect> TextureMap::GetTexture(const std::string& file) const
{
  SDL_Rect rect;
  auto it = textureMap_.find(file);
  return {it->second.get(), rect};
}

int TextureMap::GetNumberOfTextures(const std::string& animationName) const
{
  auto it = animationMap_.find(animationName);
  if (it == animationMap_.end())
  {
    return 0;
  }
  const auto& [animation, _] = it->second;
  return animation.numCols;
}

std::vector<std::string> TextureMap::GetFiles() const
{
  auto kv = std::views::keys(textureMap_);
  std::vector<std::string> files{kv.begin(), kv.end()};
  return files;
}

float TextureMap::GetTimeBetweenFrames(const std::string& animationName)
{
  auto it = animationMap_.find(animationName);
  if (it == animationMap_.end())
  {
    return 0;
  }
  const auto& [animation, _] = it->second;
  return animation.timeBetweenFrames;
}

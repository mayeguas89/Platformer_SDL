#pragma once

#include "animation.h"

#include <SDL_image.h>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

class TextureMap
{
public:
  TextureMap() = default;
  ~TextureMap();

  void SetFrameSize(const int width, const int height)
  {
    frameHeight = height;
    frameWidth = width;
  }
  void AddTexture(const std::string& filename);
  void AddAnimation(const std::string& animationName, const std::string& filename, const Animation& animation);

  std::pair<SDL_Texture*, SDL_Rect> GetTexture(const std::string& animationName, const int index) const;
  std::pair<SDL_Texture*, SDL_Rect> GetTexture(const std::string& file) const;
  int GetNumberOfTextures(const std::string& animationName) const;
  std::vector<std::string> GetFiles() const;

  // // Simple move constructor
  // TextureMap(TextureMap&& other): {}

  // // Simple move assignment operator
  // TextureMap& operator=(TextureMap&& other)
  // {
  //   for (auto it = other.textureMap_.begin(); it != other.textureMap_.end(); it++)
  //   {
  //     std::vec textureMap_[it->first] = {};
  //     for (auto texture: it->second)
  //     {
  //       textureMap_[it->first].push_back
  //     }
  //   }
  //   return *this;
  // }

  float GetTimeBetweenFrames(const std::string& animationName);

private:
  int frameWidth{0};
  int frameHeight{0};
  std::unordered_map<std::string, std::shared_ptr<SDL_Texture>> textureMap_;
  std::unordered_map<std::string, std::pair<Animation, std::shared_ptr<SDL_Texture>>> animationMap_;
};

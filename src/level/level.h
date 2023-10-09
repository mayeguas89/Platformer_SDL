#pragma once

#include "entity.h"
#include "glm/glm.hpp"

#include <SDL.h>
#include <memory>
#include <string>
#include <vector>

struct ObjectTemplate
{
  std::string name;
  glm::vec<2, int> size;
};

struct Tile
{
  enum class Type
  {
    Ground = 0,
    Wall,
    None,
  };
  std::vector<Type> types;
  // Position in the tile map - not screen position
  SDL_Rect rect;
  std::vector<std::pair<std::string, SDL_Rect>> boundingBoxes;
};

struct Layer
{
  std::vector<int> data;
};

struct TileSet
{
  int tilecount;
  int columns;
  std::shared_ptr<SDL_Texture> texture;
  std::unordered_map<int, Tile> tiles;
  std::unordered_map<std::string, ObjectTemplate> templates;
};

class Level: public Singleton<Level>
{
public:
  void SetMapSize(int width, int height);
  void SetTileSize(int width, int height);
  void AddTileSets(const std::vector<std::string>& tilesets);
  void AddLayer(const Layer& layer);
  void Draw();

  TileSet GetTileset(int index) const
  {
    return tilesets_.at(index);
  }

  glm::vec<2, int> GetTileSize() const
  {
    return {tilewidth_, tileheight_};
  }

  glm::vec<2, int> GetMapSize() const
  {
    return {width_, height_};
  }

  std::vector<Layer> GetLayers() const
  {
    return layers_;
  }

private:
  int height_{0}, width_{0};         // number of tiles
  int tilewidth_{0}, tileheight_{0}; // size of each tile
  std::vector<TileSet> tilesets_;
  std::vector<Layer> layers_;
};
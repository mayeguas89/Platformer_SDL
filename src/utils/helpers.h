#pragma once

#include "SDL_render.h"
#include "level/level.h"
#include "texture.h"
#include "texture_map.h"
#include <pugixml.hpp>

#include <fmt/core.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <ranges>
#include <string>
#include <string_view>

namespace utils
{
inline SDL_Texture* LoadTexture(const std::string& filename)
{
  auto texture = IMG_LoadTexture(SDLRender::Instance().GetRenderer(), filename.c_str());
  return texture;
}

inline void ReadBackgroundJsonFile(const std::string& filename, TextureMap& textureMap)
{
  std::ifstream f(filename);

  using json = nlohmann::json;
  json data = json::parse(f);

  // First add textures
  std::vector<std::string> files;
  if (auto layers = data.find("layers"); layers != data.end())
    files = layers.value();
  for (const auto& file: files)
  {
    textureMap.AddTexture(file);
  }
}

inline void ReadTextureMapJsonFile(const std::string& filename, TextureMap& textureMap)
{
  std::ifstream f(filename);

  using json = nlohmann::json;
  json data = json::parse(f);

  // First add textures
  std::vector<std::string> files;
  if (auto frames = data.find("files"); frames != data.end())
    files = frames.value();
  for (const auto& file: files)
  {
    textureMap.AddTexture(file);
  }

  // Set frame size
  int frameWidth{0};
  int frameHeight{0};
  int rows{0};
  int columns{0};
  if (auto it = data.find("frameWidth"); it != data.end())
    frameWidth = it.value();
  if (auto it = data.find("frameHeight"); it != data.end())
    frameHeight = it.value();
  if (auto it = data.find("rows"); it != data.end())
    rows = it.value();
  if (auto it = data.find("columns"); it != data.end())
    columns = it.value();

  textureMap.SetFrameSize(frameWidth, frameHeight);

  // Add animations
  if (auto animations = data.find("animations"); animations != data.end())
  {
    for (auto& [animationName, value]: animations.value().items())
    {
      int row{0};
      int numCols{0};
      float timeBetweenFrames{0.f};
      bool loop{false};
      int fileIndex{-1};

      if (auto it = value.find("row"); it != value.end())
        row = it.value();
      if (auto it = value.find("columns"); it != value.end())
        numCols = it.value();
      if (auto it = value.find("timeBetweenFrames"); it != value.end())
        timeBetweenFrames = it.value();
      if (auto it = value.find("loop"); it != value.end())
        loop = it.value();
      if (auto it = value.find("fileIndex"); it != value.end())
        fileIndex = it.value();
      Animation anim{timeBetweenFrames, loop, row, numCols};
      textureMap.AddAnimation(animationName, files.at(fileIndex), anim);
    }
  }
}

inline void ReadLevelFile(const std::string& filename, Level& level)
{
  using json = nlohmann::json;
  std::ifstream f(filename);
  json data = json::parse(f);
  auto path = std::filesystem::path(filename).parent_path();

  // First read map size
  int height{0}, width{0};         // number of tiles
  int tilewidth{0}, tileheight{0}; // size of each tile
  if (auto it = data.find("height"); it != data.end())
    height = it.value();
  if (auto it = data.find("width"); it != data.end())
    width = it.value();
  if (auto it = data.find("tilewidth"); it != data.end())
    tilewidth = it.value();
  if (auto it = data.find("tileheight"); it != data.end())
    tileheight = it.value();

  level.SetMapSize(width, height);
  level.SetTileSize(tilewidth, tileheight);

  // TileSets Source
  std::vector<std::string> tilesets;
  if (auto it = data.find("tilesets"); it != data.end())
  {
    for (auto& tileset: it.value())
    {
      if (auto source = tileset.find("source"); source != tileset.end())
      {
        tilesets.push_back(fmt::format("{}/{}", path.string(), source.value()));
      }
    }
  }
  level.AddTileSets(tilesets);

  // Layers
  if (auto it = data.find("layers"); it != data.end())
  {
    for (auto& layer: it.value())
    {
      Layer theLayer;
      if (auto dataIt = layer.find("data"); dataIt != layer.end())
      {
        for (auto& value: dataIt.value())
          theLayer.data.push_back(value);
      }
      level.AddLayer(theLayer);
    }
  }
}

inline void ReadTemplateFile(const std::string& filename, ObjectTemplate& objTemplate)
{
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(filename.c_str());
  if (!result)
  {
    std::string error_msg = fmt::format("Error reading the file {}\n. Error {}\n", filename, result.description());
    spdlog::info(error_msg);
    throw std::runtime_error(error_msg);
  }

  auto templateNode = doc.child("template");
  auto objNode = templateNode.child("object");
  objTemplate.name = objNode.attribute("name").as_string();
  objTemplate.size = {objNode.attribute("width").as_int(), objNode.attribute("height").as_int()};
}

inline void ReadTileSetFile(const std::string& filename, TileSet& tileset)
{
  auto path = std::filesystem::path(filename).parent_path();
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(filename.c_str());
  if (!result)
  {
    std::string error_msg = fmt::format("Error reading the file {}\n. Error {}\n", filename, result.description());
    spdlog::info(error_msg);
    throw std::runtime_error(error_msg);
  }
  auto tilesetNode = doc.child("tileset");
  tileset.tilecount = tilesetNode.attribute("tilecount").as_int();
  tileset.columns = tilesetNode.attribute("columns").as_int();
  auto image = tilesetNode.child("image").attribute("source").as_string();
  auto texture = LoadTexture(fmt::format("Assets/Tiles/{}", image));

  int tilewidth = tilesetNode.attribute("tilewidth").as_int();
  int tileheight = tilesetNode.attribute("tileheight").as_int();

  for (auto& tileNode: tilesetNode.children("tile"))
  {
    std::string tileType = tileNode.attribute("type").as_string();
    Tile tile;
    auto split = tileType | std::ranges::views::split(',')
                 | std::ranges::views::transform(
                   [](auto&& str) { return std::string_view(&*str.begin(), std::ranges::distance(str)); });
    for (const auto part: split)
    {
      auto type = Tile::Type::None;
      if (part == "Ground")
        type = Tile::Type::Ground;
      else if (part == "Wall")
        type = Tile::Type::Wall;
      tile.types.push_back(type);
    }
    int id = tileNode.attribute("id").as_int();
    // Calculate position in texture
    tile.rect.w = tilewidth;
    tile.rect.h = tileheight;
    int row = id / tileset.columns;
    int col = id - row * tileset.columns;
    tile.rect.x = tilewidth * col;
    tile.rect.y = tileheight * row;

    // Read templates and assign boundix boxes
    auto objGroupNode = tileNode.child("objectgroup");
    for (auto& object: objGroupNode.children("object"))
    {
      auto objFilename = object.attribute("template").as_string();
      int x = object.attribute("x").as_int();
      int y = object.attribute("y").as_int();
      auto it = tileset.templates.find(objFilename);
      if (it == tileset.templates.end())
      {
        ObjectTemplate objTemplate;
        ReadTemplateFile(fmt::format("{}/{}", path.string(), objFilename), objTemplate);
        it = tileset.templates.insert({objFilename, objTemplate}).first;
      }
      const auto& objTemplate = it->second;
      SDL_Rect boundingBox;
      boundingBox.x = x;
      boundingBox.y = y;
      boundingBox.w = objTemplate.size.x;
      boundingBox.h = objTemplate.size.y;
      tile.boundingBoxes.push_back({objTemplate.name, boundingBox});
    }

    tileset.tiles.insert({id, tile});
  }
  tileset.texture = std::move(std::shared_ptr<SDL_Texture>(texture, SDLTextureDestroyer{}));
}
inline bool PointInRectangle(const glm::vec2& point, const SDL_Rect& rectangle)
{
  const auto [x, y, w, h] = rectangle;
  return point.x >= x && point.x <= x + w && point.y >= y && point.y <= y + h;
}
inline bool RectangleIntersection(const SDL_Rect& test, const SDL_Rect& rectangle)
{
  const auto [x, y, w, h] = test;
  const auto [xR, yR, wR, hR] = rectangle;
  return x <= xR + wR && x + w >= xR && y <= yR + hR && y + h >= yR;
}
}
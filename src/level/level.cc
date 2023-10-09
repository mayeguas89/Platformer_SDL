#include "level.h"

#include "camera.h"
#include "sdl_render.h"
#include "texture.h"
#include "utils/helpers.h"

void Level::SetMapSize(int width, int height)
{
  width_ = width;
  height_ = height;
}

void Level::SetTileSize(int width, int height)
{
  tilewidth_ = width;
  tileheight_ = height;
}

void Level::AddTileSets(const std::vector<std::string>& tilesets)
{
  for (const auto& filename: tilesets)
  {
    TileSet tileSet;
    utils::ReadTileSetFile(filename, tileSet);
    tilesets_.push_back(tileSet);
  }
}

void Level::AddLayer(const Layer& layer)
{
  layers_.push_back(layer);
}

void Level::Draw()
{
  // Screen
  auto& render = SDLRender::Instance();
  auto screenWidth = render.GetWidth();  // 912
  auto screenHeigh = render.GetHeight(); // 480

  // Camera
  auto& camera = Camera::Instance();
  SDL_Rect view = camera.GetView(); // 10x10

  // How many pixels per tile in camera
  glm::vec<2, int> screenTile = {screenWidth / view.w, screenHeigh / view.h};

  const auto& tileset = tilesets_.at(0);
  for (const auto& layer: layers_)
  {
    // Fit whole map into screen
    // layer.data has the whole map 24x24
    // camera has to show 10x10 tiles of  view.w x view.h
    int screenIndex1D = 0;
    for (int y = view.y; y < view.h + view.y; y++)
    {
      for (int x = view.x; x < view.w + view.x; x++)
      {
        int mapIndex = y * width_ + x;
        const auto& tileIndex = layer.data.at(mapIndex);
        if (tileIndex == 0)
        {
          screenIndex1D++;
          continue;
        }
        const auto& tile = tileset.tiles.find(tileIndex - 1)->second;
        int screenIndexY = screenIndex1D / view.h;
        int screenIndexX = screenIndex1D - screenIndexY * view.w;
        SDL_Rect dest;
        dest.w = screenTile.x;
        dest.h = screenTile.y;
        dest.x = screenIndexX * screenTile.x;
        dest.y = screenIndexY * screenTile.y;
        SDL_RenderCopy(SDLRender::Instance().GetRenderer(), tileset.texture.get(), &tile.rect, &dest);
        screenIndex1D++;
      }
    }
    // for (const auto& tileIndex: layer.data)
    // {
    //   const auto& tile = tileset.tiles.find(tileIndex - 1)->second;
    //   int screenY = screenPosition / width_;
    //   int screenX = screenPosition - screenY * width_;
    //   SDL_Rect dest;
    //   dest.w = screenWidthPerTile;
    //   dest.h = screenHeightPerTile;
    //   dest.x = screenX * screenWidthPerTile;
    //   dest.y = screenY * screenHeightPerTile;
    //   // dest.w = screenWidthPerTile;
    //   // dest.h = screenHeightPerTile;
    //   // dest.x = screenX * screenWidthPerTile;
    //   // dest.y = screenY * screenHeightPerTile;
    //   SDL_RenderCopy(SDLRender::Instance().GetRenderer(), tileset.texture.get(), &tile.rect, &dest);
    //   screenPosition++;
    // }
  }
}

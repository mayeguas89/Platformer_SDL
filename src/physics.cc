#include "physics.h"

#include "camera.h"
#include "utils/helpers.h"

void Movement::operator()(std::shared_ptr<Entity> entity)
{
  auto seconds = TimeManager::deltaTime / 1000.f;
  auto currentSpeed = entity->GetCurrentSpeed();
  auto playerPosition = entity->GetPosition();
  playerPosition.x += currentSpeed.x * seconds;
  playerPosition.y += currentSpeed.y * seconds;

  if (playerPosition.x >= 0.f && playerPosition.y >= 0.f
      && playerPosition.x <= SDLRender::Instance().GetWidth() - entity->GetTexture().second.w
      && playerPosition.y <= SDLRender::Instance().GetHeight() - entity->GetTexture().second.h)
    entity->SetPosition(playerPosition);
}

void Collision::operator()(std::shared_ptr<Entity> entity)
{
  auto seconds = TimeManager::deltaTime / 1000.f;
  // Screen
  auto& render = SDLRender::Instance();
  auto screenWidth = render.GetWidth();  // 912
  auto screenHeigh = render.GetHeight(); // 480

  // Camera
  // view.w is number of tiles in x direction in camera space
  // view.h is number of tiles in y direction in camera space 20x20
  // view.x and view.y are camera top left in map coordinates {0, 4};
  auto& camera = Camera::Instance();
  auto view = camera.GetView();

  glm::vec<2, int> screenTileSize = {screenWidth / view.w, screenHeigh / view.h};

  // Player
  auto [texture, playerRect] = entity->GetTexture();
  auto topLeftPlayerPositionInScreen = entity->GetPosition();
  auto currentSpeed = entity->GetCurrentSpeed();
  topLeftPlayerPositionInScreen.x += currentSpeed.x * seconds;
  topLeftPlayerPositionInScreen.y += currentSpeed.y * seconds;

  auto state = entity->GetState();
  SDL_Rect playerBoundingBox;
  playerBoundingBox.x = topLeftPlayerPositionInScreen.x + ((state == "run") ? 16 : 18);
  playerBoundingBox.y = topLeftPlayerPositionInScreen.y + ((state == "run") ? 24 : 24);
  playerBoundingBox.w = (state == "run") ? 25 : 22;
  playerBoundingBox.h = (state == "run") ? 32 : 32;
  Physics::Instance().debugData.push_back(playerBoundingBox);
  playerRect.x = topLeftPlayerPositionInScreen.x;
  playerRect.y = topLeftPlayerPositionInScreen.y;
  Physics::Instance().debugData.push_back(playerRect);

  topLeftPlayerPositionInScreen.x = playerBoundingBox.x;
  topLeftPlayerPositionInScreen.y = playerBoundingBox.y;
  // playerRect.w is the player frame texture width
  // playerRect.h is the player frame texture height
  // Get player feet center position
  glm::vec2 feetPlayerScreenPosition = {topLeftPlayerPositionInScreen.x + 0.5f * playerBoundingBox.w,
                                        topLeftPlayerPositionInScreen.y + playerBoundingBox.h};

  // Level
  auto& level = Level::Instance();
  auto tileset = level.GetTileset(0);
  auto levelSize = level.GetMapSize(); // 24 tiles x 24 tiles
  auto tileSize = level.GetTileSize(); // 24px x 24px

  int topLeftPlayerCameraSpaceCol = topLeftPlayerPositionInScreen.x / screenTileSize.x;
  int topLeftPlayerCameraSpaceRow = topLeftPlayerPositionInScreen.y / screenTileSize.y;
  int topRightPlayerCameraSpaceCol = (topLeftPlayerPositionInScreen.x + playerBoundingBox.w) / screenTileSize.x;

  int feetCameraCol = feetPlayerScreenPosition.x / screenTileSize.x;
  int feetCameraRow = feetPlayerScreenPosition.y / screenTileSize.y;

  // If there is collision, store the tile position
  SDL_Rect groundTileOnScreenRect;
  SDL_Rect wallTileOnScreenRect;
  // Get tiles under player
  bool onGround = false;
  bool wallCollision = false;
  for (const auto layer: level.GetLayers())
  {
    // Look On the center of the feet and one under
    for (int y = 0; y < 1 && !onGround; y++)
    {
      int mapCol = std::min(view.x + feetCameraCol, levelSize.x - 1);
      int mapRow = std::min(view.y + feetCameraRow + y, levelSize.y - 1);
      int mapIndex = mapRow * levelSize.x + mapCol;
      const auto& tileIndex = layer.data.at(mapIndex);
      if (tileIndex == 0)
        continue;
      const auto& tile = tileset.tiles.find(tileIndex - 1)->second;
      if (auto it = std::find(tile.types.begin(), tile.types.end(), Tile::Type::Ground); it != tile.types.end())
      {
        for (auto [name, bb]: tile.boundingBoxes)
        {
          SDL_Rect rect;
          glm::vec2 ratio = {(screenTileSize.x / (float)(tile.rect.w)), (screenTileSize.y / (float)(tile.rect.h))};
          // Convert from map coordinates to screen coordinates
          rect.x = ((mapCol - view.x) * tileSize.x + bb.x) * ratio.x;
          rect.y = ((mapRow - view.y) * tileSize.y + bb.y) * ratio.y;
          rect.w = bb.w * ratio.x;
          rect.h = bb.h * ratio.y;
          SDL_Rect test;
          test.x = playerBoundingBox.x;
          test.y = feetPlayerScreenPosition.y;
          test.w = playerBoundingBox.w;
          test.h = 2;
          if (name == "ground" && utils::RectangleIntersection(test, rect))
          {
            onGround = true;
            groundTileOnScreenRect = rect;
            Physics::Instance().debugData.push_back(test);
            Physics::Instance().debugData.push_back(rect);
            break;
          }
        }
      }
    }

    // If looking to the left then check the left side of the top left of the character up to its feet
    int xMin = std::max(std::min(view.x + topLeftPlayerCameraSpaceCol - 1, levelSize.x - 1), 0);
    int xMax = std::min(view.x + topLeftPlayerCameraSpaceCol, levelSize.x - 1);
    int yMin = std::min(view.y + topLeftPlayerCameraSpaceRow, levelSize.y - 1);
    int yMax = std::min(view.y + feetCameraRow, levelSize.y - 1);
    // If looking to the right then check the right side of the top left + the width of the character up to its feet
    if (entity->GetLookAt() == Entity::LookAt::Right)
    {
      xMin = std::min(view.x + topRightPlayerCameraSpaceCol, levelSize.x - 1);
      xMax = std::min(view.x + topRightPlayerCameraSpaceCol + 1, levelSize.x - 1);
    }
    for (int x = xMax; x <= xMax && !wallCollision; x++)
    {
      for (int y = yMin; y < yMax && !wallCollision; y++)
      {
        int mapIndex = y * levelSize.x + x;
        const auto& tileIndex = layer.data.at(mapIndex);
        if (tileIndex == 0)
          continue;
        const auto& tile = tileset.tiles.find(tileIndex - 1)->second;
        if (auto it = std::find(tile.types.begin(), tile.types.end(), Tile::Type::Wall); it != tile.types.end())
        {
          for (auto [name, bb]: tile.boundingBoxes)
          {
            SDL_Rect rect;
            glm::vec2 ratio = {(screenTileSize.x / (float)(tile.rect.w)),
                               (screenTileSize.y / (float)(tile.rect.h))};
            // Convert from map coordinates to screen coordinates
            rect.x = ((x - view.x) * tileSize.x + bb.x) * ratio.x;
            rect.y = ((y - view.y) * tileSize.y + bb.y) * ratio.y;
            rect.w = bb.w * ratio.x;
            rect.h = bb.h * ratio.y;

            SDL_Rect test;
            test.x = (entity->GetLookAt() == Entity::LookAt::Right) ?
                       topLeftPlayerPositionInScreen.x + playerBoundingBox.w :
                       topLeftPlayerPositionInScreen.x;
            test.y = topLeftPlayerPositionInScreen.y;
            test.w = 2;
            test.h = playerBoundingBox.h;
            if (name == "wall" && utils::RectangleIntersection(test, rect))
            {
              wallTileOnScreenRect = rect;
              Physics::Instance().debugData.push_back(test);
              Physics::Instance().debugData.push_back(rect);
              wallCollision = true;
              break;
            }
          }
        }
      }
    }
  }

  entity->SetOnGround(onGround);
  auto oldSpeed = entity->GetCurrentSpeed();
  auto oldPosition = entity->GetPosition();
  if (onGround)
  {
    oldSpeed.y = 0.f;
    // Move the character to the ground
    oldPosition.y = groundTileOnScreenRect.y - playerRect.h;
  }
  if (wallCollision)
  {
    oldSpeed.x = 0.f;
    // Move the character to the wall
    // If character is looking left the texture is fliped so decrease the distance from the 
    oldPosition.x = (wallTileOnScreenRect.x + wallTileOnScreenRect.w) - (playerRect.w - playerBoundingBox.w - (state == "run") ? 16 : 18);
    if (entity->GetLookAt() == Entity::LookAt::Right)
    {
      int deltaX = playerBoundingBox.w;
      int x = ((state == "run") ? 16 : 18) + 2;
      deltaX += x;
      oldPosition.x = wallTileOnScreenRect.x - deltaX;
    }
  }
  entity->SetPosition(oldPosition);
  entity->SetCurrentSpeed(oldSpeed);

  // float numMapTilesInXScreen = screenTileSize.x / (float)tileSize.x;
  // float numMapTilesInYScreen = screenTileSize.y / (float)tileSize.y;

  // int mapIndex = screenRow * numMapTilesInYScreen + screenCol * numMapTilesInXScreen;
}

void Gravity::operator()(std::shared_ptr<Entity> entity)
{
  if (entity->IsGrounded())
  {
    gravityAcceleration = initialGravity;
    return;
  }
  const float seconds = TimeManager::deltaTime / 1000.0f;
  auto currentSpeed = entity->GetCurrentSpeed();
  spdlog::info("{}", gravityAcceleration);
  gravityAcceleration *= multiplier;
  currentSpeed.y += gravityAcceleration * seconds;
  entity->SetCurrentSpeed(currentSpeed);
  spdlog::info("{}", gravityAcceleration);
}

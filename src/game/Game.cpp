#include "game/Game.h"

#include <algorithm>
#include <cmath>

#include <raylib.h>

#include "data/Balance.h"
#include "render/Renderer.h"

Game::Game() : time_(Balance::kTicksPerSecond), world_(133742u) {
  inventory_.resize(Balance::kInventorySize);
  hotbar_.fill(Data::ItemId::None);
  hotbar_[0] = Data::ItemId::Pickaxe;
  hotbar_[1] = Data::ItemId::Axe;
  objective_ = "Objective: Mine 20 mixed resources";
}

void Game::Run() {
  SetTargetFPS(Balance::kTargetFPS);
  Renderer renderer;

  while (!WindowShouldClose()) {
    if (IsKeyPressed(KEY_ESCAPE)) break;
    input_.Update();
    if (input_.ToggleInventoryPressed()) showInventory_ = !showInventory_;
    if (input_.ToggleDebugPressed()) showDebug_ = !showDebug_;
    if (input_.ToggleMinimapPressed()) showMinimap_ = !showMinimap_;
    if (input_.HotbarPressed() >= 0) hotbarIndex_ = input_.HotbarPressed();
    zoom_ += GetMouseWheelMove() * 0.1f;
    if (zoom_ < 1.0f) zoom_ = 1.0f;
    if (zoom_ > 4.0f) zoom_ = 4.0f;

    time_.BeginFrame(GetFrameTime());
    while (time_.ShouldRunTick()) {
      Tick();
      time_.ConsumeTick();
    }

    renderer.Draw(*this, world_, (float)time_.Alpha());
  }
}

void Game::Tick() {
  UpdatePlayerMovement();
  UpdateMining();
}

void Game::UpdatePlayerMovement() {
  Vector2 axis = input_.MoveAxis();
  const float tile = (float)Balance::kTileSize;
  float step = (Balance::kPlayerSpeedTilesPerSecond * tile) / (float)Balance::kTicksPerSecond;
  Vector2 candidate = {player_.worldPos.x + axis.x * step, player_.worldPos.y + axis.y * step};
  int tx = (int)std::floor(candidate.x / tile);
  int ty = (int)std::floor(candidate.y / tile);

  if (!world_.IsBlocked(tx, ty)) {
    player_.worldPos = candidate;
  }
}

void Game::UpdateMining() {
  if (!input_.MineHeld()) {
    mining_.active = false;
    mining_.progress = 0.0f;
    return;
  }

  Vector2 mouseWorld = GetScreenToWorld2D(GetMousePosition(),
                                          Camera2D{{(float)GetScreenWidth() * 0.5f, (float)GetScreenHeight() * 0.5f},
                                                   player_.worldPos, 0.0f, zoom_});
  int tx = (int)std::floor(mouseWorld.x / Balance::kTileSize);
  int ty = (int)std::floor(mouseWorld.y / Balance::kTileSize);
  if (!world_.IsResourceTile(tx, ty)) {
    mining_.active = false;
    mining_.progress = 0.0f;
    return;
  }

  if (!mining_.active || mining_.target.x != tx || mining_.target.y != ty) {
    mining_.active = true;
    mining_.target = {tx, ty};
    mining_.progress = 0.0f;
  }

  mining_.progress += (1.0f / (float)Balance::kTicksPerSecond) / Balance::kMiningTimeSeconds;
  if (mining_.progress >= 1.0f) {
    Data::ItemId item;
    world_.MineTile(tx, ty, 1.0f, 1.0f, item);
    if (item != Data::ItemId::None) AddItem(item, 1);
    mining_.progress = 0.0f;
  }
}

bool Game::AddItem(Data::ItemId id, int amount) {
  for (auto& slot : inventory_) {
    if (slot.id == id && slot.count < Balance::kItemStackSize) {
      int can = std::min(amount, Balance::kItemStackSize - slot.count);
      slot.count += can;
      amount -= can;
      if (amount <= 0) return true;
    }
  }
  for (auto& slot : inventory_) {
    if (slot.id == Data::ItemId::None) {
      slot.id = id;
      slot.count = std::min(amount, Balance::kItemStackSize);
      amount -= slot.count;
      if (amount <= 0) return true;
    }
  }
  return false;
}

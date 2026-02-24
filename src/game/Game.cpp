#include "game/Game.h"

#include <algorithm>
#include <cmath>

#include <raylib.h>

#include "data/Balance.h"
#include "data/Recipes.h"
#include "render/Renderer.h"

Game::Game() : time_(Balance::kTicksPerSecond), world_(133742u) {
  inventory_.resize(Balance::kInventorySize);
  hotbar_.fill(Data::ItemId::None);
  hotbar_[0] = Data::ItemId::Pickaxe;
  hotbar_[1] = Data::ItemId::Axe;
  hotbar_[2] = Data::ItemId::Furnace;
  hotbar_[3] = Data::ItemId::Chest;
  objective_ = "Objective: Craft a furnace and smelt plates";
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
    if (input_.RecipeNextPressed()) recipeIndex_ = std::min(recipeIndex_ + 1, (int)Data::GetRecipes().size() - 1);
    if (input_.RecipePrevPressed()) recipeIndex_ = std::max(recipeIndex_ - 1, 0);
    if (input_.RotatePressed()) rotation_ = (rotation_ + 1) % 4;

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
  UpdateCrafting();
  UpdateBuilding();
  UpdateFurnaces();
}

void Game::UpdatePlayerMovement() {
  Vector2 axis = input_.MoveAxis();
  const float tile = (float)Balance::kTileSize;
  float step = (Balance::kPlayerSpeedTilesPerSecond * tile) / (float)Balance::kTicksPerSecond;
  Vector2 candidate = {player_.worldPos.x + axis.x * step, player_.worldPos.y + axis.y * step};
  int tx = (int)std::floor(candidate.x / tile);
  int ty = (int)std::floor(candidate.y / tile);

  if (!world_.IsBlocked(tx, ty) && buildings_.find(IVec2{tx, ty}) == buildings_.end()) {
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

  if (buildings_.find(IVec2{tx, ty}) != buildings_.end()) {
    if (input_.DeconstructHeld()) {
      auto building = buildings_[IVec2{tx, ty}];
      AddItem(building.type == BuildingType::Furnace ? Data::ItemId::Furnace : Data::ItemId::Chest, 1);
      buildings_.erase(IVec2{tx, ty});
    }
    mining_.active = false;
    return;
  }

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

void Game::UpdateCrafting() {
  if (!showInventory_ || !input_.CraftPressed()) return;
  const auto& recipe = Data::GetRecipes()[recipeIndex_];
  if (!recipe.handCraftable) return;

  for (const auto& ing : recipe.ingredients) {
    if (CountItem(ing.item) < ing.count) return;
  }
  for (const auto& ing : recipe.ingredients) {
    RemoveItem(ing.item, ing.count);
  }
  AddItem(recipe.result.item, recipe.result.count);
}

void Game::UpdateBuilding() {
  if (!input_.PlacePressed()) return;
  Vector2 mouseWorld = GetScreenToWorld2D(GetMousePosition(),
                                          Camera2D{{(float)GetScreenWidth() * 0.5f, (float)GetScreenHeight() * 0.5f},
                                                   player_.worldPos, 0.0f, zoom_});
  int tx = (int)std::floor(mouseWorld.x / Balance::kTileSize);
  int ty = (int)std::floor(mouseWorld.y / Balance::kTileSize);

  if (world_.IsBlocked(tx, ty) || buildings_.find(IVec2{tx, ty}) != buildings_.end()) return;

  const auto selected = hotbar_[hotbarIndex_];
  if (selected == Data::ItemId::Furnace && RemoveItem(Data::ItemId::Furnace, 1)) {
    buildings_[IVec2{tx, ty}] = Building{BuildingType::Furnace, rotation_, std::vector<ItemStack>(3), 0.0f};
  } else if (selected == Data::ItemId::Chest && RemoveItem(Data::ItemId::Chest, 1)) {
    buildings_[IVec2{tx, ty}] = Building{BuildingType::Chest, rotation_, std::vector<ItemStack>(10), 0.0f};
  }
}

void Game::UpdateFurnaces() {
  for (auto& [pos, b] : buildings_) {
    (void)pos;
    if (b.type != BuildingType::Furnace) continue;
    bool canIron = CountItem(Data::ItemId::IronOre) > 0 && CountItem(Data::ItemId::Coal) > 0;
    bool canCopper = CountItem(Data::ItemId::CopperOre) > 0 && CountItem(Data::ItemId::Coal) > 0;
    if (!canIron && !canCopper) continue;

    b.progress += 1.0f / (float)Balance::kTicksPerSecond;
    if (b.progress < 2.5f) continue;
    b.progress = 0.0f;
    RemoveItem(Data::ItemId::Coal, 1);
    if (canIron) {
      RemoveItem(Data::ItemId::IronOre, 1);
      AddItem(Data::ItemId::IronPlate, 1);
    } else {
      RemoveItem(Data::ItemId::CopperOre, 1);
      AddItem(Data::ItemId::CopperPlate, 1);
    }
  }
}

int Game::CountItem(Data::ItemId id) const {
  int total = 0;
  for (const auto& slot : inventory_) {
    if (slot.id == id) total += slot.count;
  }
  return total;
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

bool Game::RemoveItem(Data::ItemId id, int amount) {
  if (CountItem(id) < amount) return false;
  for (auto& slot : inventory_) {
    if (slot.id != id || amount <= 0) continue;
    int take = std::min(slot.count, amount);
    slot.count -= take;
    amount -= take;
    if (slot.count == 0) slot.id = Data::ItemId::None;
  }
  return amount == 0;
}

#include "game/Game.h"

#include <algorithm>
#include <cmath>
#include <vector>

#include <raylib.h>

#include "data/Balance.h"
#include "data/Recipes.h"
#include "render/Renderer.h"

namespace {
IVec2 DirFromRot(int rot) {
  switch (rot % 4) {
    case 0:
      return {1, 0};
    case 1:
      return {0, 1};
    case 2:
      return {-1, 0};
    default:
      return {0, -1};
  }
}
} // namespace

Game::Game() : time_(Balance::kTicksPerSecond), world_(133742u) {
  inventory_.resize(Balance::kInventorySize);
  hotbar_.fill(Data::ItemId::None);
  hotbar_[0] = Data::ItemId::Pickaxe;
  hotbar_[1] = Data::ItemId::Axe;
  hotbar_[2] = Data::ItemId::Furnace;
  hotbar_[3] = Data::ItemId::Chest;
  hotbar_[4] = Data::ItemId::Belt;
  hotbar_[5] = Data::ItemId::Inserter;
  hotbar_[6] = Data::ItemId::BurnerGenerator;
  hotbar_[7] = Data::ItemId::PoweredDrill;
  objective_ = "Objective: Build basic automation and unlock Tier 2 tech";
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
    zoom_ = std::clamp(zoom_, 1.0f, 4.0f);

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
  UpdateAutomation();
  UpdatePowerAndResearch();
}

void Game::UpdatePlayerMovement() {
  Vector2 axis = input_.MoveAxis();
  const float tile = (float)Balance::kTileSize;
  float step = (Balance::kPlayerSpeedTilesPerSecond * tile) / (float)Balance::kTicksPerSecond;
  Vector2 candidate = {player_.worldPos.x + axis.x * step, player_.worldPos.y + axis.y * step};
  int tx = (int)std::floor(candidate.x / tile);
  int ty = (int)std::floor(candidate.y / tile);
  if (!world_.IsBlocked(tx, ty) && buildings_.find(IVec2{tx, ty}) == buildings_.end()) player_.worldPos = candidate;
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

  auto bIt = buildings_.find({tx, ty});
  if (bIt != buildings_.end()) {
    if (input_.DeconstructHeld()) {
      Data::ItemId refund = Data::ItemId::Chest;
      if (bIt->second.type == BuildingType::Furnace) refund = Data::ItemId::Furnace;
      if (bIt->second.type == BuildingType::Belt) refund = Data::ItemId::Belt;
      if (bIt->second.type == BuildingType::Inserter) refund = Data::ItemId::Inserter;
      if (bIt->second.type == BuildingType::BurnerGenerator) refund = Data::ItemId::BurnerGenerator;
      if (bIt->second.type == BuildingType::PoweredDrill) refund = Data::ItemId::PoweredDrill;
      AddItem(refund, 1);
      buildings_.erase(bIt);
      threat_ = std::max(0.0f, threat_ - 0.4f);
    }
    mining_.active = false;
    mining_.progress = 0.0f;
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

  const float mineSpeed = powerBufferSec_ > 0 ? 1.35f : 1.0f;
  mining_.progress += ((1.0f / (float)Balance::kTicksPerSecond) / Balance::kMiningTimeSeconds) * mineSpeed;
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
  for (const auto& ing : recipe.ingredients)
    if (CountItem(ing.item) < ing.count) return;
  for (const auto& ing : recipe.ingredients) RemoveItem(ing.item, ing.count);
  AddItem(recipe.result.item, recipe.result.count);
}

void Game::UpdateBuilding() {
  if (!input_.PlacePressed()) return;
  Vector2 mouseWorld = GetScreenToWorld2D(GetMousePosition(),
                                          Camera2D{{(float)GetScreenWidth() * 0.5f, (float)GetScreenHeight() * 0.5f},
                                                   player_.worldPos, 0.0f, zoom_});
  int tx = (int)std::floor(mouseWorld.x / Balance::kTileSize);
  int ty = (int)std::floor(mouseWorld.y / Balance::kTileSize);

  if (world_.IsBlocked(tx, ty) || buildings_.find({tx, ty}) != buildings_.end()) return;

  const auto selected = hotbar_[hotbarIndex_];
  auto place = [&](Data::ItemId item, BuildingType type, int invSlots = 0) {
    if (!RemoveItem(item, 1)) return;
    buildings_[{tx, ty}] = Building{type, rotation_, std::vector<ItemStack>(invSlots), 0.0f, Data::ItemId::None};
    threat_ += 0.7f;
  };

  if (selected == Data::ItemId::Furnace) place(Data::ItemId::Furnace, BuildingType::Furnace, 3);
  if (selected == Data::ItemId::Chest) place(Data::ItemId::Chest, BuildingType::Chest, 10);
  if (selected == Data::ItemId::Belt) place(Data::ItemId::Belt, BuildingType::Belt);
  if (selected == Data::ItemId::Inserter) place(Data::ItemId::Inserter, BuildingType::Inserter);
  if (selected == Data::ItemId::BurnerGenerator) place(Data::ItemId::BurnerGenerator, BuildingType::BurnerGenerator, 2);
  if (selected == Data::ItemId::PoweredDrill) place(Data::ItemId::PoweredDrill, BuildingType::PoweredDrill, 2);
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

void Game::UpdateAutomation() {
  std::vector<std::pair<IVec2, Data::ItemId>> moves;
  for (auto& [pos, b] : buildings_) {
    if (b.type != BuildingType::Belt || b.beltItem == Data::ItemId::None) continue;
    IVec2 d = DirFromRot(b.rotation);
    IVec2 next{pos.x + d.x, pos.y + d.y};
    auto nIt = buildings_.find(next);
    if (nIt != buildings_.end() && nIt->second.type == BuildingType::Belt && nIt->second.beltItem == Data::ItemId::None) {
      moves.push_back({next, b.beltItem});
      b.beltItem = Data::ItemId::None;
    }

    int ptx = (int)std::floor(player_.worldPos.x / Balance::kTileSize);
    int pty = (int)std::floor(player_.worldPos.y / Balance::kTileSize);
    if (ptx == pos.x && pty == pos.y && b.beltItem != Data::ItemId::None) {
      AddItem(b.beltItem, 1);
      b.beltItem = Data::ItemId::None;
    }
  }
  for (const auto& m : moves) buildings_[m.first].beltItem = m.second;

  for (auto& [pos, b] : buildings_) {
    if (b.type != BuildingType::Inserter) continue;
    b.progress += 1.0f / (float)Balance::kTicksPerSecond;
    if (b.progress < 0.8f) continue;
    b.progress = 0.0f;

    IVec2 d = DirFromRot(b.rotation);
    IVec2 out{pos.x + d.x, pos.y + d.y};
    auto outIt = buildings_.find(out);
    if (outIt == buildings_.end() || outIt->second.type != BuildingType::Belt || outIt->second.beltItem != Data::ItemId::None)
      continue;

    Data::ItemId moved = Data::ItemId::None;
    if (RemoveItem(Data::ItemId::IronPlate, 1)) moved = Data::ItemId::IronPlate;
    else if (RemoveItem(Data::ItemId::CopperPlate, 1)) moved = Data::ItemId::CopperPlate;
    else if (RemoveItem(Data::ItemId::Coal, 1)) moved = Data::ItemId::Coal;
    if (moved != Data::ItemId::None) outIt->second.beltItem = moved;
  }
}

void Game::UpdatePowerAndResearch() {
  if (powerBufferSec_ > 0.0f) powerBufferSec_ -= 1.0f / (float)Balance::kTicksPerSecond;

  for (auto& [pos, b] : buildings_) {
    (void)pos;
    if (b.type != BuildingType::BurnerGenerator) continue;
    if (powerBufferSec_ < 3.0f && RemoveItem(Data::ItemId::Coal, 1)) powerBufferSec_ += 4.0f;
  }

  bool hasPower = powerBufferSec_ > 0.0f;
  for (auto& [pos, b] : buildings_) {
    if (b.type != BuildingType::PoweredDrill || !hasPower) continue;
    b.progress += 1.0f / (float)Balance::kTicksPerSecond;
    if (b.progress < 1.2f) continue;
    b.progress = 0.0f;
    if (!world_.IsResourceTile(pos.x, pos.y)) continue;
    Data::ItemId item;
    world_.MineTile(pos.x, pos.y, 1.0f, 1.0f, item);
    if (item != Data::ItemId::None) AddItem(item, 1);
  }

  while (CountItem(Data::ItemId::SciencePack) > 0 && !tech2Unlocked_) {
    RemoveItem(Data::ItemId::SciencePack, 1);
    science_ += 1;
    if (science_ >= 10) {
      tech2Unlocked_ = true;
      objective_ = "Tier 2 unlocked! TODO Milestone 7: survive enemy waves";
    }
  }
}

int Game::CountItem(Data::ItemId id) const {
  int total = 0;
  for (const auto& slot : inventory_) if (slot.id == id) total += slot.count;
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

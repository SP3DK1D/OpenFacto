#pragma once

#include <array>
#include <string>
#include <vector>

#include <raylib.h>

#include "core/Input.h"
#include "core/Time.h"
#include "data/Balance.h"
#include "data/Items.h"
#include "world/World.h"

struct ItemStack {
  Data::ItemId id = Data::ItemId::None;
  int count = 0;
};

struct PlayerState {
  Vector2 worldPos = {0.f, 0.f};
  float hp = 100.f;
  float energy = 100.f;
};

struct MiningState {
  bool active = false;
  IVec2 target{0, 0};
  float progress = 0.f;
};

class Game {
 public:
  Game();
  void Run();

  const PlayerState& GetPlayer() const { return player_; }
  const std::vector<ItemStack>& GetInventory() const { return inventory_; }
  int GetHotbarIndex() const { return hotbarIndex_; }
  bool ShowDebug() const { return showDebug_; }
  bool ShowInventory() const { return showInventory_; }
  bool ShowMinimap() const { return showMinimap_; }
  const MiningState& GetMining() const { return mining_; }
  const std::string& GetObjective() const { return objective_; }
  float GetZoom() const { return zoom_; }

 private:
  void Tick();
  void UpdatePlayerMovement();
  void UpdateMining();
  bool AddItem(Data::ItemId id, int amount);

  Time time_;
  Input input_;
  World world_;

  PlayerState player_;
  MiningState mining_;
  std::vector<ItemStack> inventory_;
  std::array<Data::ItemId, Balance::kHotbarSize> hotbar_;
  int hotbarIndex_ = 0;
  bool showDebug_ = false;
  bool showInventory_ = false;
  bool showMinimap_ = false;
  float zoom_ = 2.0f;
  std::string objective_;
};

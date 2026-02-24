#pragma once

#include <array>
#include <string>
#include <unordered_map>
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

enum class BuildingType { Furnace, Chest, Belt, Inserter, BurnerGenerator, PoweredDrill, Wall, Turret };

struct Building {
  BuildingType type;
  int rotation = 0;
  std::vector<ItemStack> inventory;
  float progress = 0.0f;
  Data::ItemId beltItem = Data::ItemId::None;
  float hp = 100.0f;
};

struct Enemy {
  Vector2 pos{0, 0};
  Vector2 dir{1, 0};
  float hp = 35.0f;
  float flash = 0.0f;
  float attackCd = 0.0f;
};

struct Nest {
  IVec2 tile{0, 0};
  float spawnTimer = 5.0f;
  float hp = 150.0f;
};

struct Objective {
  std::string text;
  bool completed = false;
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
  bool ShowGuide() const { return showGuide_; }
  const MiningState& GetMining() const { return mining_; }
  const std::string& GetObjective() const { return objective_; }
  float GetZoom() const { return zoom_; }
  int GetRecipeIndex() const { return recipeIndex_; }
  int GetRotation() const { return rotation_; }
  const std::unordered_map<IVec2, Building>& GetBuildings() const { return buildings_; }
  const std::vector<Enemy>& GetEnemies() const { return enemies_; }
  const std::vector<Nest>& GetNests() const { return nests_; }
  float GetThreat() const { return threat_; }
  float GetPollution() const { return pollution_; }
  int GetScience() const { return science_; }
  bool IsTech2Unlocked() const { return tech2Unlocked_; }
  const std::string& GetRecipeSearch() const { return recipeSearch_; }
  const std::vector<Objective>& GetObjectives() const { return objectives_; }
  const std::string& GetNotification() const { return notification_; }

 private:
  void Tick();
  void InitNests();
  void UpdatePlayerMovement();
  void UpdateMining();
  void UpdateCrafting();
  void UpdateBuilding();
  void UpdateFurnaces();
  void UpdateAutomation();
  void UpdatePowerAndResearch();
  void UpdateEnemies();
  void UpdateCombat();
  void UpdateObjectives();
  void HandleTextInput();
  void LoadConfig();
  void SaveGame(const std::string& path);
  void LoadGame(const std::string& path);
  bool AddItem(Data::ItemId id, int amount);
  bool RemoveItem(Data::ItemId id, int amount);
  int CountItem(Data::ItemId id) const;

  Time time_;
  Input input_;
  World world_;

  PlayerState player_;
  MiningState mining_;
  std::vector<ItemStack> inventory_;
  std::array<Data::ItemId, Balance::kHotbarSize> hotbar_;
  std::unordered_map<IVec2, Building> buildings_;
  std::vector<Enemy> enemies_;
  std::vector<Nest> nests_;
  std::vector<Objective> objectives_;
  int hotbarIndex_ = 0;
  bool showDebug_ = false;
  bool showInventory_ = false;
  bool showMinimap_ = false;
  bool showGuide_ = false;
  float zoom_ = 2.0f;
  int recipeIndex_ = 0;
  int rotation_ = 0;
  float threat_ = 0.0f;
  float pollution_ = 0.0f;
  int science_ = 0;
  bool tech2Unlocked_ = false;
  float powerBufferSec_ = 0.0f;
  float autosaveTimer_ = 0.0f;
  float simSpeed_ = 1.0f;
  bool autosaveEnabled_ = true;
  std::string recipeSearch_;
  std::string notification_;
  float notificationTimer_ = 0.0f;
  std::string objective_;
};

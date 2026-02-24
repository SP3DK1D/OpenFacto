#include "game/Game.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
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

float DistSq(Vector2 a, Vector2 b) {
  float dx = a.x - b.x;
  float dy = a.y - b.y;
  return dx * dx + dy * dy;
}
} // namespace

Game::Game() : time_(Balance::kTicksPerSecond), world_(133742u) {
  LoadConfig();
  inventory_.resize(Balance::kInventorySize);
  hotbar_.fill(Data::ItemId::None);
  hotbar_[0] = Data::ItemId::Pickaxe;
  hotbar_[1] = Data::ItemId::Wall;
  hotbar_[2] = Data::ItemId::Furnace;
  hotbar_[3] = Data::ItemId::Chest;
  hotbar_[4] = Data::ItemId::Belt;
  hotbar_[5] = Data::ItemId::Inserter;
  hotbar_[6] = Data::ItemId::BurnerGenerator;
  hotbar_[7] = Data::ItemId::PoweredDrill;
  hotbar_[8] = Data::ItemId::Turret;

  objectives_ = {{"Craft a Furnace", false}, {"Smelt 10 Iron Plates", false}, {"Place Belt + Inserter", false},
                 {"Research Tier 2 (10 science)", false}, {"Clear 1 Nest", false}, {"Survive first wave", false}};
  objective_ = objectives_.front().text;
  InitNests();
}

void Game::InitNests() {
  nests_.clear();
  for (int i = 0; i < 8; ++i) {
    int x = ((i % 4) - 2) * 24 + 40;
    int y = ((i / 4) * 40) - 20;
    if (std::abs(x) < 10 || std::abs(y) < 10) x += 30;
    nests_.push_back({{x, y}, 8.0f + i, 160.0f});
  }
}

void Game::Run() {
  SetTargetFPS(Balance::kTargetFPS);
  Renderer renderer;

  while (!WindowShouldClose()) {
    if (IsKeyPressed(KEY_ESCAPE)) break;
    input_.Update();
    HandleTextInput();

    if (input_.ToggleInventoryPressed()) showInventory_ = !showInventory_;
    if (input_.ToggleDebugPressed()) showDebug_ = !showDebug_;
    if (input_.ToggleMinimapPressed()) showMinimap_ = !showMinimap_;
    if (input_.ToggleGuidePressed()) showGuide_ = !showGuide_;
    if (input_.HotbarPressed() >= 0) hotbarIndex_ = input_.HotbarPressed();
    if (input_.RecipeNextPressed()) recipeIndex_ = std::min(recipeIndex_ + 1, (int)Data::GetRecipes().size() - 1);
    if (input_.RecipePrevPressed()) recipeIndex_ = std::max(recipeIndex_ - 1, 0);
    if (input_.RotatePressed()) rotation_ = (rotation_ + 1) % 4;
    if (input_.SavePressed()) SaveGame("savegame.ofs");
    if (input_.LoadPressed()) LoadGame("savegame.ofs");

    zoom_ += GetMouseWheelMove() * 0.1f;
    zoom_ = std::clamp(zoom_, 1.0f, 4.0f);

    const float frame = GetFrameTime() * simSpeed_;
    time_.BeginFrame(frame);
    while (time_.ShouldRunTick()) {
      Tick();
      time_.ConsumeTick();
    }

    renderer.Draw(*this, world_, (float)time_.Alpha());
  }
}

void Game::Tick() {
  if (notificationTimer_ > 0) notificationTimer_ -= 1.0f / Balance::kTicksPerSecond;
  else notification_.clear();

  UpdatePlayerMovement();
  UpdateMining();
  UpdateCrafting();
  UpdateBuilding();
  UpdateFurnaces();
  UpdateAutomation();
  UpdatePowerAndResearch();
  UpdateEnemies();
  UpdateCombat();
  UpdateObjectives();

  if (autosaveEnabled_) {
    autosaveTimer_ += 1.0f / Balance::kTicksPerSecond;
    if (autosaveTimer_ > 20.0f) {
      SaveGame("autosave.ofs");
      autosaveTimer_ = 0.0f;
    }
  }
}

void Game::HandleTextInput() {
  if (!showInventory_) return;
  int ch = GetCharPressed();
  while (ch > 0) {
    if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == ' ') recipeSearch_.push_back((char)ch);
    ch = GetCharPressed();
  }
  if (IsKeyPressed(KEY_BACKSPACE) && !recipeSearch_.empty()) recipeSearch_.pop_back();
}

void Game::UpdatePlayerMovement() {
  Vector2 axis = input_.MoveAxis();
  const float tile = (float)Balance::kTileSize;
  float step = (Balance::kPlayerSpeedTilesPerSecond * tile) / (float)Balance::kTicksPerSecond;
  Vector2 candidate = {player_.worldPos.x + axis.x * step, player_.worldPos.y + axis.y * step};
  int tx = (int)std::floor(candidate.x / tile);
  int ty = (int)std::floor(candidate.y / tile);
  auto it = buildings_.find({tx, ty});
  bool blockedByBuilding = (it != buildings_.end() && (it->second.type == BuildingType::Wall || it->second.type == BuildingType::Turret));
  if (!world_.IsBlocked(tx, ty) && !blockedByBuilding) player_.worldPos = candidate;
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
      if (bIt->second.type == BuildingType::Wall) refund = Data::ItemId::Wall;
      if (bIt->second.type == BuildingType::Turret) refund = Data::ItemId::Turret;
      AddItem(refund, 1);
      buildings_.erase(bIt);
      threat_ = std::max(0.0f, threat_ - 0.4f);
    }
    mining_.active = false;
    mining_.progress = 0.0f;
    return;
  }

  for (auto& nest : nests_) {
    if (nest.tile.x == tx && nest.tile.y == ty) {
      mining_.active = true;
      mining_.target = {tx, ty};
      mining_.progress += 1.0f / (float)Balance::kTicksPerSecond;
      if (mining_.progress >= 1.0f) {
        nest.hp -= 20.0f;
        mining_.progress = 0.0f;
      }
      return;
    }
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
  if (!recipeSearch_.empty() && recipe.name.find(recipeSearch_) == std::string::npos) return;
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
  auto place = [&](Data::ItemId item, BuildingType type, int invSlots = 0, float hp = 80.0f) {
    if (!RemoveItem(item, 1)) return;
    buildings_[{tx, ty}] = Building{type, rotation_, std::vector<ItemStack>(invSlots), 0.0f, Data::ItemId::None, hp};
    threat_ += 0.7f;
  };

  if (selected == Data::ItemId::Furnace) place(Data::ItemId::Furnace, BuildingType::Furnace, 3);
  if (selected == Data::ItemId::Chest) place(Data::ItemId::Chest, BuildingType::Chest, 10);
  if (selected == Data::ItemId::Belt) place(Data::ItemId::Belt, BuildingType::Belt, 0, 30.0f);
  if (selected == Data::ItemId::Inserter) place(Data::ItemId::Inserter, BuildingType::Inserter, 0, 45.0f);
  if (selected == Data::ItemId::BurnerGenerator) place(Data::ItemId::BurnerGenerator, BuildingType::BurnerGenerator, 2);
  if (selected == Data::ItemId::PoweredDrill) place(Data::ItemId::PoweredDrill, BuildingType::PoweredDrill, 2);
  if (selected == Data::ItemId::Wall) place(Data::ItemId::Wall, BuildingType::Wall, 0, 160.0f);
  if (selected == Data::ItemId::Turret) place(Data::ItemId::Turret, BuildingType::Turret, 2, 120.0f);
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
      notification_ = "Tech unlocked: Tier 2";
      notificationTimer_ = 3.0f;
    }
  }

  pollution_ += (float)buildings_.size() * 0.0015f;
  pollution_ *= 0.9992f;
  threat_ += pollution_ * 0.002f + (tech2Unlocked_ ? 0.01f : 0.0f);
}

void Game::UpdateEnemies() {
  for (auto& nest : nests_) {
    nest.spawnTimer -= 1.0f / (float)Balance::kTicksPerSecond;
    float rate = std::max(2.0f, 8.0f - threat_ * 0.05f);
    if (nest.spawnTimer <= 0.0f) {
      nest.spawnTimer = rate;
      enemies_.push_back({{nest.tile.x * (float)Balance::kTileSize + 8, nest.tile.y * (float)Balance::kTileSize + 8},
                          {1, 0}, 30.0f + threat_ * 0.2f, 0.0f, 0.0f});
    }
  }

  Vector2 player = player_.worldPos;
  for (auto& e : enemies_) {
    if (e.attackCd > 0) e.attackCd -= 1.0f / (float)Balance::kTicksPerSecond;
    if (e.flash > 0) e.flash -= 1.0f / (float)Balance::kTicksPerSecond;

    Vector2 target = player;
    float best = DistSq(e.pos, player);
    for (const auto& [p, b] : buildings_) {
      if (b.type != BuildingType::BurnerGenerator && b.type != BuildingType::PoweredDrill && b.type != BuildingType::Turret)
        continue;
      Vector2 bp{p.x * (float)Balance::kTileSize + 16, p.y * (float)Balance::kTileSize + 16};
      float d = DistSq(e.pos, bp);
      if (d < best && d < 220 * 220) {
        best = d;
        target = bp;
      }
    }

    Vector2 dir{target.x - e.pos.x, target.y - e.pos.y};
    if (Vector2LengthSqr(dir) > 1) dir = Vector2Normalize(dir);
    float speed = 42.0f / (float)Balance::kTicksPerSecond;
    Vector2 cand{e.pos.x + dir.x * speed, e.pos.y + dir.y * speed};
    int tx = (int)std::floor(cand.x / Balance::kTileSize);
    int ty = (int)std::floor(cand.y / Balance::kTileSize);
    auto bit = buildings_.find({tx, ty});
    bool blocked = world_.IsBlocked(tx, ty) || (bit != buildings_.end() && (bit->second.type == BuildingType::Wall));
    if (!blocked) {
      e.pos = cand;
    } else {
      int altx = (int)std::floor((e.pos.x + dir.y * speed) / Balance::kTileSize);
      int alty = (int)std::floor((e.pos.y - dir.x * speed) / Balance::kTileSize);
      if (!world_.IsBlocked(altx, alty)) {
        e.pos.x += dir.y * speed;
        e.pos.y -= dir.x * speed;
      }
    }

    if (DistSq(e.pos, player) < 22 * 22 && e.attackCd <= 0) {
      player_.hp -= 6.0f;
      e.attackCd = 0.7f;
    }
  }

  enemies_.erase(std::remove_if(enemies_.begin(), enemies_.end(), [](const Enemy& e) { return e.hp <= 0; }), enemies_.end());
  nests_.erase(std::remove_if(nests_.begin(), nests_.end(), [](const Nest& n) { return n.hp <= 0; }), nests_.end());

  if (player_.hp <= 0.0f) {
    player_.hp = 100.0f;
    player_.worldPos = {0, 0};
    for (auto& slot : inventory_) slot.count /= 2;
    notification_ = "You died. Respawned at base with item penalty.";
    notificationTimer_ = 4.0f;
  }
}

void Game::UpdateCombat() {
  if (input_.AttackPressed()) {
    for (auto& e : enemies_) {
      if (DistSq(e.pos, player_.worldPos) < 44 * 44) {
        e.hp -= 18.0f;
        e.flash = 0.15f;
      }
    }
  }

  for (auto& [pos, b] : buildings_) {
    if (b.type != BuildingType::Turret) continue;
    b.progress += 1.0f / Balance::kTicksPerSecond;
    if (b.progress < 0.3f) continue;
    b.progress = 0.0f;
    if (!RemoveItem(Data::ItemId::Ammo, 1)) continue;

    Enemy* nearest = nullptr;
    float best = 120 * 120;
    Vector2 bp{pos.x * (float)Balance::kTileSize + 16, pos.y * (float)Balance::kTileSize + 16};
    for (auto& e : enemies_) {
      float d = DistSq(bp, e.pos);
      if (d < best) {
        best = d;
        nearest = &e;
      }
    }
    if (nearest) {
      nearest->hp -= 12.0f;
      nearest->flash = 0.12f;
    }
  }
}

void Game::UpdateObjectives() {
  if (!objectives_[0].completed && CountItem(Data::ItemId::Furnace) > 0) {
    objectives_[0].completed = true;
    notification_ = "Objective complete: Craft a Furnace";
    notificationTimer_ = 3.0f;
  }
  if (!objectives_[1].completed && CountItem(Data::ItemId::IronPlate) >= 10) objectives_[1].completed = true;
  if (!objectives_[2].completed && CountItem(Data::ItemId::Belt) > 0 && CountItem(Data::ItemId::Inserter) > 0)
    objectives_[2].completed = true;
  if (!objectives_[3].completed && tech2Unlocked_) objectives_[3].completed = true;
  if (!objectives_[4].completed && nests_.size() < 8) objectives_[4].completed = true;
  if (!objectives_[5].completed && threat_ > 25.0f && player_.hp > 0) objectives_[5].completed = true;

  for (const auto& o : objectives_) {
    if (!o.completed) {
      objective_ = o.text;
      return;
    }
  }
  objective_ = "All milestone objectives complete. Expand your factory.";
}

void Game::LoadConfig() {
  std::ifstream in("config.ini");
  if (!in) return;
  std::string line;
  while (std::getline(in, line)) {
    if (line.rfind("autosave=", 0) == 0) autosaveEnabled_ = (line.substr(9) != "0");
    if (line.rfind("sim_speed=", 0) == 0) simSpeed_ = std::clamp(std::stof(line.substr(10)), 0.5f, 2.0f);
  }
}

void Game::SaveGame(const std::string& path) {
  std::ofstream out(path);
  if (!out) return;
  out << "seed " << world_.GetSeed() << '\n';
  out << "player " << player_.worldPos.x << ' ' << player_.worldPos.y << ' ' << player_.hp << ' ' << player_.energy << '\n';
  out << "stats " << threat_ << ' ' << pollution_ << ' ' << science_ << ' ' << tech2Unlocked_ << '\n';
  out << "inv " << inventory_.size() << '\n';
  for (const auto& s : inventory_) out << (int)s.id << ' ' << s.count << '\n';
  out << "build " << buildings_.size() << '\n';
  for (const auto& [p, b] : buildings_)
    out << p.x << ' ' << p.y << ' ' << (int)b.type << ' ' << b.rotation << ' ' << b.progress << ' ' << (int)b.beltItem << ' '
        << b.hp << '\n';
  out << "nest " << nests_.size() << '\n';
  for (const auto& n : nests_) out << n.tile.x << ' ' << n.tile.y << ' ' << n.spawnTimer << ' ' << n.hp << '\n';
}

void Game::LoadGame(const std::string& path) {
  std::ifstream in(path);
  if (!in) return;
  std::string token;
  size_t n = 0;
  while (in >> token) {
    if (token == "seed") {
      unsigned seed = 0;
      in >> seed;
      world_ = World(seed);
    } else if (token == "player") {
      in >> player_.worldPos.x >> player_.worldPos.y >> player_.hp >> player_.energy;
    } else if (token == "stats") {
      in >> threat_ >> pollution_ >> science_ >> tech2Unlocked_;
    } else if (token == "inv") {
      in >> n;
      inventory_.assign(n, {});
      for (size_t i = 0; i < n; ++i) {
        int id = 0;
        in >> id >> inventory_[i].count;
        inventory_[i].id = (Data::ItemId)id;
      }
    } else if (token == "build") {
      in >> n;
      buildings_.clear();
      for (size_t i = 0; i < n; ++i) {
        IVec2 p{};
        Building b{};
        int t = 0, belt = 0;
        in >> p.x >> p.y >> t >> b.rotation >> b.progress >> belt >> b.hp;
        b.type = (BuildingType)t;
        b.beltItem = (Data::ItemId)belt;
        buildings_[p] = b;
      }
    } else if (token == "nest") {
      in >> n;
      nests_.clear();
      for (size_t i = 0; i < n; ++i) {
        Nest ns;
        in >> ns.tile.x >> ns.tile.y >> ns.spawnTimer >> ns.hp;
        nests_.push_back(ns);
      }
    }
  }
  notification_ = "Loaded save: " + path;
  notificationTimer_ = 2.5f;
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

#include "world/World.h"

#include <algorithm>

World::World(uint32_t seed) : seed_(seed) {}

WorldGen::TileType World::GetTile(int tx, int ty) {
  IVec2 k{tx, ty};
  auto it = cache_.find(k);
  if (it != cache_.end()) return it->second;

  auto tile = WorldGen::GenerateTile(tx, ty, seed_);
  if (std::abs(tx) < 6 && std::abs(ty) < 6 && tile == WorldGen::TileType::Water) {
    tile = WorldGen::TileType::Grass;
  }
  cache_[k] = tile;
  if (tile == WorldGen::TileType::Iron || tile == WorldGen::TileType::Copper ||
      tile == WorldGen::TileType::Coal || tile == WorldGen::TileType::Stone ||
      tile == WorldGen::TileType::Tree) {
    resources_[k] = ResourceState{8};
  }
  return tile;
}

bool World::IsBlocked(int tx, int ty) { return GetTile(tx, ty) == WorldGen::TileType::Water; }

bool World::IsResourceTile(int tx, int ty) {
  const auto t = GetTile(tx, ty);
  return t == WorldGen::TileType::Iron || t == WorldGen::TileType::Copper ||
         t == WorldGen::TileType::Coal || t == WorldGen::TileType::Stone ||
         t == WorldGen::TileType::Tree;
}

float World::MineTile(int tx, int ty, float deltaSec, float speedMultiplier, Data::ItemId& outItem) {
  outItem = Data::ItemId::None;
  IVec2 k{tx, ty};
  auto rit = resources_.find(k);
  if (rit == resources_.end() || rit->second.remaining <= 0) return 0.0f;

  float progress = deltaSec * speedMultiplier;
  if (progress < 1.0f) return progress;

  rit->second.remaining = std::max(0, rit->second.remaining - 1);
  const auto tile = GetTile(tx, ty);
  if (tile == WorldGen::TileType::Iron) outItem = Data::ItemId::IronOre;
  if (tile == WorldGen::TileType::Copper) outItem = Data::ItemId::CopperOre;
  if (tile == WorldGen::TileType::Coal) outItem = Data::ItemId::Coal;
  if (tile == WorldGen::TileType::Stone) outItem = Data::ItemId::Stone;
  if (tile == WorldGen::TileType::Tree) outItem = Data::ItemId::Wood;
  if (rit->second.remaining == 0) cache_[k] = WorldGen::TileType::Grass;
  return 1.0f;
}

uint32_t World::GetSeed() const { return seed_; }

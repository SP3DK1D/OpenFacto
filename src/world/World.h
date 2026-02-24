#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

#include "data/Items.h"
#include "world/Gen.h"

struct IVec2 {
  int x;
  int y;
  bool operator==(const IVec2& o) const { return x == o.x && y == o.y; }
};

namespace std {
template <>
struct hash<IVec2> {
  size_t operator()(const IVec2& v) const noexcept {
    return (static_cast<size_t>(v.x) << 32) ^ static_cast<uint32_t>(v.y);
  }
};
} // namespace std

struct ResourceState {
  int remaining = 0;
};

class World {
 public:
  explicit World(uint32_t seed);
  WorldGen::TileType GetTile(int tx, int ty);
  bool IsBlocked(int tx, int ty);
  bool IsResourceTile(int tx, int ty);
  float MineTile(int tx, int ty, float deltaSec, float speedMultiplier, Data::ItemId& outItem);
  uint32_t GetSeed() const;

 private:
  uint32_t seed_;
  std::unordered_map<IVec2, WorldGen::TileType> cache_;
  std::unordered_map<IVec2, ResourceState> resources_;
};

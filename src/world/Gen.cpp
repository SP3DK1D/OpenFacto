#include "world/Gen.h"

namespace WorldGen {
uint32_t Hash2D(int x, int y, uint32_t seed) {
  uint32_t h = seed;
  h ^= static_cast<uint32_t>(x) * 374761393u;
  h = (h << 13) ^ h;
  h ^= static_cast<uint32_t>(y) * 668265263u;
  h = (h << 13) ^ h;
  return h * 1274126177u;
}

TileType GenerateTile(int worldX, int worldY, uint32_t seed) {
  const auto v = Hash2D(worldX, worldY, seed) % 1000;
  const auto patch = Hash2D(worldX / 4, worldY / 4, seed + 99) % 1000;

  if (v < 120) return TileType::Water;
  if (v < 180) return TileType::Sand;

  if (patch < 35) return TileType::Iron;
  if (patch < 60) return TileType::Copper;
  if (patch < 90) return TileType::Coal;
  if (patch < 120) return TileType::Stone;
  if (patch < 150) return TileType::Tree;
  return TileType::Grass;
}
} // namespace WorldGen

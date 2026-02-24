#pragma once

#include <cstdint>

namespace WorldGen {

enum class TileType { Grass, Sand, Water, Iron, Copper, Coal, Stone, Tree };

uint32_t Hash2D(int x, int y, uint32_t seed);
TileType GenerateTile(int worldX, int worldY, uint32_t seed);

} // namespace WorldGen

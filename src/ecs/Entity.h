#pragma once
#include <cstdint>

namespace ECS {
using EntityId = std::uint32_t;
struct Position {
  float x;
  float y;
};
} // namespace ECS

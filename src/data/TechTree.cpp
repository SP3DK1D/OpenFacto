#include "data/TechTree.h"

namespace Data {
const std::vector<TechNode>& GetTechTree() {
  static const std::vector<TechNode> tree = {
      {"Tier 0: Hand mining + crafting", true},
      {"Tier 1: Furnace smelting", false},
      {"Tier 2: Logistics + power", false},
      {"Tier 3: Assembly + defense", false},
  };
  return tree;
}
} // namespace Data

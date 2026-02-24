#pragma once
#include <string>
#include <vector>

namespace Data {
struct TechNode {
  std::string name;
  bool unlocked;
};
const std::vector<TechNode>& GetTechTree();
} // namespace Data

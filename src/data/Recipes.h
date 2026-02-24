#pragma once
#include <string>
#include <vector>

namespace Data {
struct Recipe {
  std::string name;
};
const std::vector<Recipe>& GetRecipes();
} // namespace Data

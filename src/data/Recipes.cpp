#include "data/Recipes.h"

namespace Data {
const std::vector<Recipe>& GetRecipes() {
  static const std::vector<Recipe> recipes = {
      {"TODO Milestone 3: hand crafting"},
      {"TODO Milestone 3: furnace smelting"},
      {"TODO Milestones 5-8: automation/research/defense"}};
  return recipes;
}
} // namespace Data

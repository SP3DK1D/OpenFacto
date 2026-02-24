#pragma once
#include <string>
#include <vector>

#include "data/Items.h"

namespace Data {

struct Ingredient {
  ItemId item;
  int count;
};

struct Recipe {
  std::string name;
  std::vector<Ingredient> ingredients;
  Ingredient result;
  float craftTime;
  bool handCraftable;
  bool furnaceOnly;
};

const std::vector<Recipe>& GetRecipes();

} // namespace Data

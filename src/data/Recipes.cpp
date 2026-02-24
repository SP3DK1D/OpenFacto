#include "data/Recipes.h"

namespace Data {
const std::vector<Recipe>& GetRecipes() {
  static const std::vector<Recipe> recipes = {
      {"Furnace", {{ItemId::Stone, 8}}, {ItemId::Furnace, 1}, 1.0f, true, false},
      {"Chest", {{ItemId::Wood, 8}}, {ItemId::Chest, 1}, 0.8f, true, false},
      {"Copper Wire", {{ItemId::CopperPlate, 1}}, {ItemId::CopperWire, 2}, 0.6f, true, false},
      {"Iron Gear", {{ItemId::IronPlate, 2}}, {ItemId::IronGear, 1}, 1.0f, true, false},
      {"Basic Circuit", {{ItemId::CopperWire, 2}, {ItemId::IronPlate, 1}}, {ItemId::BasicCircuit, 1}, 1.4f, true,
       false},
      {"Smelt Iron", {{ItemId::IronOre, 1}, {ItemId::Coal, 1}}, {ItemId::IronPlate, 1}, 2.5f, false, true},
      {"Smelt Copper", {{ItemId::CopperOre, 1}, {ItemId::Coal, 1}}, {ItemId::CopperPlate, 1}, 2.5f, false, true},
  };
  return recipes;
}
} // namespace Data

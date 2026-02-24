#include "data/Recipes.h"

namespace Data {
const std::vector<Recipe>& GetRecipes() {
  static const std::vector<Recipe> recipes = {
      {"Furnace", {{ItemId::Stone, 8}}, {ItemId::Furnace, 1}, 1.0f, true, false},
      {"Chest", {{ItemId::Wood, 8}}, {ItemId::Chest, 1}, 0.8f, true, false},
      {"Belt", {{ItemId::IronPlate, 1}}, {ItemId::Belt, 2}, 0.5f, true, false},
      {"Inserter", {{ItemId::IronGear, 1}, {ItemId::IronPlate, 1}}, {ItemId::Inserter, 1}, 1.2f, true, false},
      {"Burner Generator", {{ItemId::IronPlate, 4}, {ItemId::Stone, 2}}, {ItemId::BurnerGenerator, 1}, 1.4f, true,
       false},
      {"Powered Drill", {{ItemId::IronPlate, 5}, {ItemId::IronGear, 2}}, {ItemId::PoweredDrill, 1}, 1.6f, true,
       false},
      {"Copper Wire", {{ItemId::CopperPlate, 1}}, {ItemId::CopperWire, 2}, 0.6f, true, false},
      {"Iron Gear", {{ItemId::IronPlate, 2}}, {ItemId::IronGear, 1}, 1.0f, true, false},
      {"Basic Circuit", {{ItemId::CopperWire, 2}, {ItemId::IronPlate, 1}}, {ItemId::BasicCircuit, 1}, 1.4f, true,
       false},
      {"Science Pack", {{ItemId::CopperWire, 1}, {ItemId::IronGear, 1}}, {ItemId::SciencePack, 1}, 1.0f, true,
       false},
      {"Smelt Iron", {{ItemId::IronOre, 1}, {ItemId::Coal, 1}}, {ItemId::IronPlate, 1}, 2.5f, false, true},
      {"Smelt Copper", {{ItemId::CopperOre, 1}, {ItemId::Coal, 1}}, {ItemId::CopperPlate, 1}, 2.5f, false, true},
  };
  return recipes;
}
} // namespace Data

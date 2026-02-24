#include "data/Items.h"

#include <array>

namespace Data {

static const std::array<ItemDef, 23> kItems = {{{ItemId::None, "None", 0, false},
                                                {ItemId::IronOre, "Iron Ore", 100, false},
                                                {ItemId::CopperOre, "Copper Ore", 100, false},
                                                {ItemId::Coal, "Coal", 100, false},
                                                {ItemId::Stone, "Stone", 100, false},
                                                {ItemId::Wood, "Wood", 100, false},
                                                {ItemId::IronPlate, "Iron Plate", 100, false},
                                                {ItemId::CopperPlate, "Copper Plate", 100, false},
                                                {ItemId::IronGear, "Iron Gear", 100, false},
                                                {ItemId::CopperWire, "Copper Wire", 200, false},
                                                {ItemId::BasicCircuit, "Basic Circuit", 100, false},
                                                {ItemId::SciencePack, "Science Pack", 100, false},
                                                {ItemId::Ammo, "Ammo", 200, false},
                                                {ItemId::Pickaxe, "Pickaxe", 1, false},
                                                {ItemId::Axe, "Axe", 1, false},
                                                {ItemId::Furnace, "Furnace", 50, true},
                                                {ItemId::Chest, "Chest", 50, true},
                                                {ItemId::Belt, "Belt", 200, true},
                                                {ItemId::Inserter, "Inserter", 100, true},
                                                {ItemId::BurnerGenerator, "Burner Generator", 20, true},
                                                {ItemId::PoweredDrill, "Powered Drill", 50, true},
                                                {ItemId::Wall, "Wall", 200, true},
                                                {ItemId::Turret, "Turret", 100, true}}};

const ItemDef& GetItem(ItemId id) { return kItems[static_cast<size_t>(id)]; }

const std::vector<ItemDef>& GetAllItems() {
  static const std::vector<ItemDef> v(kItems.begin(), kItems.end());
  return v;
}

} // namespace Data

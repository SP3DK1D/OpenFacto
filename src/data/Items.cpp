#include "data/Items.h"

#include <array>

namespace Data {

static const std::array<ItemDef, 9> kItems = {{{ItemId::None, "None", 0},
                                               {ItemId::IronOre, "Iron Ore", 100},
                                               {ItemId::CopperOre, "Copper Ore", 100},
                                               {ItemId::Coal, "Coal", 100},
                                               {ItemId::Stone, "Stone", 100},
                                               {ItemId::Wood, "Wood", 100},
                                               {ItemId::Pickaxe, "Pickaxe", 1},
                                               {ItemId::Axe, "Axe", 1},
                                               {ItemId::Chest, "Chest", 50}}};

const ItemDef& GetItem(ItemId id) { return kItems[static_cast<size_t>(id)]; }

const std::vector<ItemDef>& GetAllItems() {
  static const std::vector<ItemDef> v(kItems.begin(), kItems.end());
  return v;
}

} // namespace Data

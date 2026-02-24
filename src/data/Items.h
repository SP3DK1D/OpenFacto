#pragma once

#include <string>
#include <vector>

namespace Data {

enum class ItemId {
  None = 0,
  IronOre,
  CopperOre,
  Coal,
  Stone,
  Wood,
  Pickaxe,
  Axe,
  Chest
};

struct ItemDef {
  ItemId id;
  std::string name;
  int maxStack;
};

const ItemDef& GetItem(ItemId id);
const std::vector<ItemDef>& GetAllItems();

} // namespace Data

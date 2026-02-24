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
  IronPlate,
  CopperPlate,
  IronGear,
  CopperWire,
  BasicCircuit,
  Pickaxe,
  Axe,
  Furnace,
  Chest
};

struct ItemDef {
  ItemId id;
  std::string name;
  int maxStack;
  bool placeable;
};

const ItemDef& GetItem(ItemId id);
const std::vector<ItemDef>& GetAllItems();

} // namespace Data

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
  SciencePack,
  Ammo,
  Pickaxe,
  Axe,
  Furnace,
  Chest,
  Belt,
  Inserter,
  BurnerGenerator,
  PoweredDrill,
  Wall,
  Turret
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

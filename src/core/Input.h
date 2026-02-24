#pragma once

#include <raylib.h>

class Input {
 public:
  void Update();
  Vector2 MoveAxis() const;
  bool MineHeld() const;
  bool PlacePressed() const;
  bool DeconstructHeld() const;
  bool RotatePressed() const;
  bool CraftPressed() const;
  bool AttackPressed() const;
  bool SavePressed() const;
  bool LoadPressed() const;
  bool ToggleGuidePressed() const;
  int HotbarPressed() const;
  bool RecipeNextPressed() const;
  bool RecipePrevPressed() const;
  bool ToggleInventoryPressed() const;
  bool ToggleDebugPressed() const;
  bool ToggleMinimapPressed() const;

 private:
  int hotbarPressed_ = -1;
};

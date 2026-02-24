#pragma once

#include <raylib.h>

class Input {
 public:
  void Update();
  Vector2 MoveAxis() const;
  bool MineHeld() const;
  int HotbarPressed() const;
  bool ToggleInventoryPressed() const;
  bool ToggleDebugPressed() const;
  bool ToggleMinimapPressed() const;

 private:
  int hotbarPressed_ = -1;
};

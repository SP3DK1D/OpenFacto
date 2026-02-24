#pragma once
#include "game/Game.h"

class Renderer {
 public:
  void Draw(const Game& game, World& world, float alpha);

 private:
  float zoom_ = 2.0f;
};

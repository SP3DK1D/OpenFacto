#include <raylib.h>

#include "data/Balance.h"
#include "game/Game.h"

int main() {
  InitWindow(Balance::kScreenWidth, Balance::kScreenHeight, "OpenFacto - Milestone 1+2");
  SetExitKey(KEY_NULL);

  Game game;
  game.Run();

  CloseWindow();
  return 0;
}

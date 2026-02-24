#include "ui/UI.h"

#include <raylib.h>

#include "data/Items.h"

void UI::DrawHUD(const Game& game) {
  const auto& p = game.GetPlayer();
  DrawRectangle(16, 16, 260, 74, Fade(BLACK, 0.55f));
  DrawText(TextFormat("HP: %.0f", p.hp), 24, 24, 20, RAYWHITE);
  DrawText(TextFormat("Energy: %.0f", p.energy), 24, 48, 20, RAYWHITE);
  DrawText(game.GetObjective().c_str(), 16, 96, 20, GOLD);

  const int hotbarY = GetScreenHeight() - 54;
  for (int i = 0; i < 9; ++i) {
    Rectangle r{(float)(GetScreenWidth() / 2 - 9 * 22 + i * 44), (float)hotbarY, 40, 40};
    DrawRectangleRec(r, i == game.GetHotbarIndex() ? Fade(GOLD, 0.8f) : Fade(BLACK, 0.7f));
    DrawRectangleLinesEx(r, 2, RAYWHITE);
    DrawText(TextFormat("%d", i + 1), (int)r.x + 2, (int)r.y + 2, 12, LIGHTGRAY);
  }

  if (game.ShowInventory()) {
    DrawRectangle(120, 120, GetScreenWidth() - 240, GetScreenHeight() - 240, Fade(BLACK, 0.75f));
    DrawText("Inventory (TAB)", 140, 136, 24, RAYWHITE);
    const auto& inv = game.GetInventory();
    for (int i = 0; i < (int)inv.size(); ++i) {
      int x = 140 + (i % 10) * 100;
      int y = 180 + (i / 10) * 64;
      DrawRectangleLines(x, y, 96, 56, GRAY);
      if (inv[i].id != Data::ItemId::None) {
        DrawText(Data::GetItem(inv[i].id).name.c_str(), x + 6, y + 8, 14, RAYWHITE);
        DrawText(TextFormat("x%d", inv[i].count), x + 6, y + 28, 14, SKYBLUE);
      }
    }
  }
}

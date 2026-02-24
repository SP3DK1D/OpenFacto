#include "ui/UI.h"

#include <raylib.h>

#include "data/Items.h"
#include "data/Recipes.h"

void UI::DrawHUD(const Game& game) {
  const auto& p = game.GetPlayer();
  DrawRectangle(16, 16, 430, 78, Fade(BLACK, 0.55f));
  DrawText(TextFormat("HP: %.0f", p.hp), 24, 24, 20, RAYWHITE);
  DrawText(TextFormat("Energy: %.0f", p.energy), 24, 48, 20, RAYWHITE);
  DrawText(game.GetObjective().c_str(), 16, 100, 20, GOLD);

  const int hotbarY = GetScreenHeight() - 54;
  for (int i = 0; i < 9; ++i) {
    Rectangle r{(float)(GetScreenWidth() / 2 - 9 * 22 + i * 44), (float)hotbarY, 40, 40};
    DrawRectangleRec(r, i == game.GetHotbarIndex() ? Fade(GOLD, 0.8f) : Fade(BLACK, 0.7f));
    DrawRectangleLinesEx(r, 2, RAYWHITE);
    DrawText(TextFormat("%d", i + 1), (int)r.x + 2, (int)r.y + 2, 12, LIGHTGRAY);
  }

  if (game.ShowInventory()) {
    DrawRectangle(90, 90, GetScreenWidth() - 180, GetScreenHeight() - 180, Fade(BLACK, 0.78f));
    DrawText("Inventory + Crafting (UP/DOWN + ENTER)", 112, 110, 24, RAYWHITE);
    const auto& inv = game.GetInventory();
    for (int i = 0; i < (int)inv.size(); ++i) {
      int x = 112 + (i % 10) * 76;
      int y = 150 + (i / 10) * 56;
      DrawRectangleLines(x, y, 70, 50, GRAY);
      if (inv[i].id != Data::ItemId::None) {
        DrawText(Data::GetItem(inv[i].id).name.c_str(), x + 4, y + 6, 12, RAYWHITE);
        DrawText(TextFormat("x%d", inv[i].count), x + 4, y + 24, 12, SKYBLUE);
      }
    }

    int rx = GetScreenWidth() - 420;
    int ry = 150;
    DrawText("Recipes", rx, ry - 30, 20, GOLD);
    const auto& recipes = Data::GetRecipes();
    for (int i = 0; i < (int)recipes.size(); ++i) {
      Color c = (i == game.GetRecipeIndex()) ? YELLOW : LIGHTGRAY;
      DrawText(recipes[i].name.c_str(), rx, ry + i * 22, 18, c);
    }
  }
}

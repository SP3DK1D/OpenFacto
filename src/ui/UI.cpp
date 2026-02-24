#include "ui/UI.h"

#include <raylib.h>

#include "data/Items.h"
#include "data/Recipes.h"

void UI::DrawHUD(const Game& game) {
  const auto& p = game.GetPlayer();
  DrawRectangle(16, 16, 520, 78, Fade(BLACK, 0.55f));
  DrawText(TextFormat("HP: %.0f", p.hp), 24, 24, 20, RAYWHITE);
  DrawText(TextFormat("Energy: %.0f", p.energy), 24, 48, 20, RAYWHITE);
  DrawText(game.GetObjective().c_str(), 16, 100, 20, GOLD);

  const auto& objectives = game.GetObjectives();
  int oy = 128;
  for (int i = 0; i < (int)objectives.size(); ++i) {
    DrawText(TextFormat("[%c] %s", objectives[i].completed ? 'x' : ' ', objectives[i].text.c_str()), 16, oy + i * 16, 14,
             objectives[i].completed ? GREEN : LIGHTGRAY);
  }

  const int hotbarY = GetScreenHeight() - 54;
  for (int i = 0; i < 9; ++i) {
    Rectangle r{(float)(GetScreenWidth() / 2 - 9 * 22 + i * 44), (float)hotbarY, 40, 40};
    DrawRectangleRec(r, i == game.GetHotbarIndex() ? Fade(GOLD, 0.8f) : Fade(BLACK, 0.7f));
    DrawRectangleLinesEx(r, 2, RAYWHITE);
    DrawText(TextFormat("%d", i + 1), (int)r.x + 2, (int)r.y + 2, 12, LIGHTGRAY);
  }

  if (!game.GetNotification().empty()) {
    int w = MeasureText(game.GetNotification().c_str(), 20);
    DrawRectangle(GetScreenWidth() / 2 - w / 2 - 12, 20, w + 24, 34, Fade(BLACK, 0.7f));
    DrawText(game.GetNotification().c_str(), GetScreenWidth() / 2 - w / 2, 28, 20, YELLOW);
  }

  if (game.ShowInventory()) {
    DrawRectangle(90, 90, GetScreenWidth() - 180, GetScreenHeight() - 180, Fade(BLACK, 0.78f));
    DrawText("Inventory + Crafting (UP/DOWN + ENTER) | Search:", 112, 110, 24, RAYWHITE);
    DrawText(game.GetRecipeSearch().c_str(), 700, 116, 20, SKYBLUE);
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
      bool matches = game.GetRecipeSearch().empty() || recipes[i].name.find(game.GetRecipeSearch()) != std::string::npos;
      if (!matches) continue;
      Color c = (i == game.GetRecipeIndex()) ? YELLOW : LIGHTGRAY;
      DrawText(recipes[i].name.c_str(), rx, ry, 18, c);
      ry += 22;
    }
  }

  if (game.ShowGuide()) {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.88f));
    DrawText("GUIDE (G to close)", 40, 30, 36, GOLD);
    DrawText("Controls: WASD move, LMB mine, RMB place, R rotate, Space melee, F5/F9 save/load", 40, 90, 20, RAYWHITE);
    DrawText("Progression: Mine ore -> Smelt plates -> Craft belts/inserters -> Power drills -> Science", 40, 124, 20,
             RAYWHITE);
    DrawText("Belts/Inserts: inserters push items onto belts, belts move forward each tick", 40, 158, 20, RAYWHITE);
    DrawText("Power: burner generators consume coal into power buffer, drills consume buffered power", 40, 192, 20,
             RAYWHITE);
    DrawText("Research: science packs are consumed automatically to unlock Tier 2 at 10 packs", 40, 226, 20, RAYWHITE);
    DrawText("Enemies: nests spawn biters over time; threat and pollution increase attack pressure", 40, 260, 20,
             RAYWHITE);
    DrawText("Defense: walls block enemies, turrets consume ammo and auto-fire in range", 40, 294, 20, RAYWHITE);
  }
}

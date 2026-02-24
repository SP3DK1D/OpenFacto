#include "render/Renderer.h"

#include <raylib.h>

#include "data/Balance.h"
#include "ui/UI.h"

namespace {
Color TileColor(WorldGen::TileType t) {
  switch (t) {
    case WorldGen::TileType::Grass:
      return {70, 130, 70, 255};
    case WorldGen::TileType::Sand:
      return {210, 190, 120, 255};
    case WorldGen::TileType::Water:
      return {50, 110, 180, 255};
    case WorldGen::TileType::Iron:
      return {110, 110, 150, 255};
    case WorldGen::TileType::Copper:
      return {170, 100, 60, 255};
    case WorldGen::TileType::Coal:
      return {60, 60, 60, 255};
    case WorldGen::TileType::Stone:
      return {130, 130, 130, 255};
    case WorldGen::TileType::Tree:
      return {40, 90, 35, 255};
  }
  return MAGENTA;
}
} // namespace

void Renderer::Draw(const Game& game, World& world, float /*alpha*/) {
  const auto& player = game.GetPlayer();
  Camera2D cam{};
  cam.target = player.worldPos;
  cam.offset = {(float)GetScreenWidth() * 0.5f, (float)GetScreenHeight() * 0.5f};
  zoom_ = game.GetZoom();
  cam.zoom = zoom_;

  BeginDrawing();
  ClearBackground({26, 30, 38, 255});
  BeginMode2D(cam);

  const int tile = Balance::kTileSize;
  int radiusX = GetScreenWidth() / tile / 2 + 4;
  int radiusY = GetScreenHeight() / tile / 2 + 4;
  int ptx = (int)floorf(player.worldPos.x / tile);
  int pty = (int)floorf(player.worldPos.y / tile);

  for (int y = pty - radiusY; y <= pty + radiusY; ++y) {
    for (int x = ptx - radiusX; x <= ptx + radiusX; ++x) {
      auto tt = world.GetTile(x, y);
      DrawRectangle(x * tile, y * tile, tile, tile, TileColor(tt));
      DrawRectangleLines(x * tile, y * tile, tile, tile, Fade(BLACK, 0.12f));
    }
  }

  for (const auto& n : game.GetNests()) {
    DrawCircle(n.tile.x * tile + 16, n.tile.y * tile + 16, 12, MAROON);
    DrawRectangle(n.tile.x * tile + 2, n.tile.y * tile - 4, (int)((tile - 4) * (n.hp / 160.0f)), 3, RED);
  }

  for (const auto& [pos, b] : game.GetBuildings()) {
    Color c = BROWN;
    if (b.type == BuildingType::Furnace) c = DARKGRAY;
    if (b.type == BuildingType::Belt) c = {70, 70, 90, 255};
    if (b.type == BuildingType::Inserter) c = {100, 120, 170, 255};
    if (b.type == BuildingType::BurnerGenerator) c = {150, 80, 50, 255};
    if (b.type == BuildingType::PoweredDrill) c = {100, 100, 40, 255};
    if (b.type == BuildingType::Wall) c = GRAY;
    if (b.type == BuildingType::Turret) c = DARKBLUE;
    DrawRectangle(pos.x * tile + 3, pos.y * tile + 3, tile - 6, tile - 6, c);

    if (b.type == BuildingType::Furnace || b.type == BuildingType::Inserter || b.type == BuildingType::PoweredDrill ||
        b.type == BuildingType::Turret) {
      DrawRectangle(pos.x * tile + 3, pos.y * tile + tile - 5, (int)((tile - 6) * (b.progress / 2.5f)), 3, ORANGE);
    }
    if (b.type == BuildingType::Belt && b.beltItem != Data::ItemId::None) {
      DrawCircle(pos.x * tile + tile / 2, pos.y * tile + tile / 2, 5, YELLOW);
    }
  }

  for (const auto& e : game.GetEnemies()) {
    Color col = e.flash > 0 ? RED : GREEN;
    DrawCircle((int)e.pos.x, (int)e.pos.y, 8, col);
  }

  DrawRectangle((int)player.worldPos.x - 10, (int)player.worldPos.y - 10, 20, 20, YELLOW);
  const auto& mine = game.GetMining();
  if (mine.active) {
    DrawRectangleLines(mine.target.x * tile, mine.target.y * tile, tile, tile, GOLD);
    DrawRectangle(mine.target.x * tile, mine.target.y * tile - 6, (int)(tile * mine.progress), 4, LIME);
  }

  EndMode2D();

  UI ui;
  ui.DrawHUD(game);

  if (game.ShowMinimap()) {
    DrawRectangle(GetScreenWidth() - 210, 10, 200, 200, Fade(BLACK, 0.6f));
    DrawText(TextFormat("Pollution %.2f", game.GetPollution()), GetScreenWidth() - 200, 20, 16, RAYWHITE);
    DrawText(TextFormat("Nests %d", (int)game.GetNests().size()), GetScreenWidth() - 200, 42, 16, RAYWHITE);
  }
  if (game.ShowDebug()) {
    DrawRectangle(8, GetScreenHeight() - 184, 560, 176, Fade(BLACK, 0.6f));
    DrawText(TextFormat("Seed: %u", world.GetSeed()), 16, GetScreenHeight() - 176, 18, SKYBLUE);
    DrawText(TextFormat("FPS: %d", GetFPS()), 16, GetScreenHeight() - 152, 18, SKYBLUE);
    DrawText(TextFormat("Buildings: %d  Enemies: %d  Nests: %d", (int)game.GetBuildings().size(),
                        (int)game.GetEnemies().size(), (int)game.GetNests().size()),
             16, GetScreenHeight() - 128, 18, SKYBLUE);
    DrawText(TextFormat("Threat: %.1f Pollution: %.2f", game.GetThreat(), game.GetPollution()), 16,
             GetScreenHeight() - 104, 18, SKYBLUE);
    DrawText(TextFormat("Science: %d | Tier2: %s", game.GetScience(), game.IsTech2Unlocked() ? "yes" : "no"), 16,
             GetScreenHeight() - 80, 18, SKYBLUE);
    DrawText("Space=melee, F5 save, F9 load, G guide", 16, GetScreenHeight() - 56, 18, ORANGE);
    DrawText("M8 includes objectives, notifications, recipe search, save/load, config.ini", 16,
             GetScreenHeight() - 32, 16, ORANGE);
  }

  EndDrawing();
}

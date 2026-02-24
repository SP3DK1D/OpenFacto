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

  for (const auto& [pos, b] : game.GetBuildings()) {
    Color c = BROWN;
    if (b.type == BuildingType::Furnace) c = DARKGRAY;
    if (b.type == BuildingType::Belt) c = {70, 70, 90, 255};
    if (b.type == BuildingType::Inserter) c = {100, 120, 170, 255};
    if (b.type == BuildingType::BurnerGenerator) c = {150, 80, 50, 255};
    if (b.type == BuildingType::PoweredDrill) c = {100, 100, 40, 255};
    DrawRectangle(pos.x * tile + 3, pos.y * tile + 3, tile - 6, tile - 6, c);

    if (b.type == BuildingType::Furnace || b.type == BuildingType::Inserter || b.type == BuildingType::PoweredDrill) {
      DrawRectangle(pos.x * tile + 3, pos.y * tile + tile - 5, (int)((tile - 6) * (b.progress / 2.5f)), 3, ORANGE);
    }
    if (b.type == BuildingType::Belt && b.beltItem != Data::ItemId::None) {
      DrawCircle(pos.x * tile + tile / 2, pos.y * tile + tile / 2, 5, YELLOW);
    }
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
    DrawText("Minimap TODO", GetScreenWidth() - 190, 90, 20, RAYWHITE);
  }
  if (game.ShowDebug()) {
    DrawRectangle(8, GetScreenHeight() - 160, 520, 152, Fade(BLACK, 0.6f));
    DrawText(TextFormat("Seed: %u", world.GetSeed()), 16, GetScreenHeight() - 152, 18, SKYBLUE);
    DrawText(TextFormat("FPS: %d", GetFPS()), 16, GetScreenHeight() - 128, 18, SKYBLUE);
    DrawText(TextFormat("Buildings: %d", (int)game.GetBuildings().size()), 16, GetScreenHeight() - 104, 18, SKYBLUE);
    DrawText(TextFormat("Threat: %.1f", game.GetThreat()), 16, GetScreenHeight() - 80, 18, SKYBLUE);
    DrawText(TextFormat("Science: %d | Tier2: %s", game.GetScience(), game.IsTech2Unlocked() ? "yes" : "no"), 16,
             GetScreenHeight() - 56, 18, SKYBLUE);
    DrawText("TODO M7-M8: enemies/waves/defense + full guide", 16, GetScreenHeight() - 32, 18, ORANGE);
  }

  EndDrawing();
}

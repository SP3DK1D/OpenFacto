# OpenFacto

OpenFacto is a C++/raylib 2D tile-based survival + automation prototype inspired by Factorio.

This version implements **Milestone 1–6** baseline systems:
- Fixed-tick simulation (20 TPS) + render loop (60 FPS target)
- Deterministic procedural world generation
- Smooth WASD movement + water/building collision
- Mining with hold-to-mine progress bar and inventory stacking
- Hand crafting panel (recipes list)
- Placeable buildings (furnace/chest/belt/inserter/generator/drill), rotation, deconstruction
- Furnace smelting loop (ore + coal => plates)
- Basic automation (belts move items, inserters feed belts)
- Simple power model (burner generators consume coal to power boosted mining and powered drills)
- Research starter loop (science packs consumed to unlock Tier 2)

## Build

### Linux/macOS
```bash
cmake -S . -B build
cmake --build build -j
./build/openfacto
```

### Windows (PowerShell, Visual Studio generator)
```powershell
cmake -S . -B build -G "Visual Studio 17 2022"
cmake --build build --config Release
.\build\Release\openfacto.exe
```

Raylib is fetched automatically with CMake FetchContent.

## Double-click start files
- Windows: `start_game.bat`
- Linux: `start_game.sh` (may require `chmod +x start_game.sh` once)
- macOS: `start_game.command` (double-clickable from Finder)

## Controls
- `WASD`: move
- Mouse wheel: zoom
- `Left Mouse` (hold): mine resource tile
- `Right Mouse` (press): place selected placeable item from hotbar
- `F` + target building while holding LMB: deconstruct building
- `R`: rotate placement orientation
- `UP/DOWN`: select crafting recipe
- `ENTER` (inventory open): craft selected hand recipe
- `1..9`: hotbar slot select
- `TAB`: inventory/crafting panel
- `M`: minimap toggle
- `` ` ``: debug overlay toggle
- `ESC`: close window

## Known TODOs
- Milestone 7: enemies, nests, waves, basic defenses
- Milestone 8: full in-game guide screen, objective director, balancing pass, optional save/load

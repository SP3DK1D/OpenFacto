# OpenFacto

OpenFacto is a C++/raylib 2D tile-based survival + automation prototype inspired by Factorio.

This version implements **Milestone 1–4** baseline systems:
- Fixed-tick simulation (20 TPS) + render loop (60 FPS target)
- Deterministic procedural world generation
- Smooth WASD movement + water/building collision
- Camera follow + zoom
- Mining with hold-to-mine progress bar and inventory stacking
- Hand crafting panel (recipes list)
- Placeable buildings (furnace/chest), rotation, deconstruction
- Furnace smelting loop (ore + coal => plates)

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

These scripts build (if needed) then run the game.

## Controls
- `WASD`: move
- Mouse wheel: zoom
- `Left Mouse` (hold): mine resource tile
- `Right Mouse` (press): place selected placeable item from hotbar
- `F` + mine targeting a building: deconstruct building
- `R`: rotate placement orientation
- `UP/DOWN`: select crafting recipe
- `ENTER` (inventory open): craft selected hand recipe
- `1..9`: hotbar slot select
- `TAB`: inventory/crafting panel
- `M`: minimap toggle
- `` ` ``: debug overlay toggle
- `ESC`: close window

## Troubleshooting
- If raylib download fails, verify internet access and rerun configure.
- If graphics context creation fails, run in a desktop session with GPU/display access.
- If CMake cache gets stale after toolchain changes, delete `build/` and reconfigure.

## TODO Roadmap (Milestones 5-8)
- Milestone 5: belts + inserters automation
- Milestone 6: power network + drills + research/tech progression
- Milestone 7: enemy nests + wave pressure + defenses
- Milestone 8: full guide/objective flow + balancing + optional save/load

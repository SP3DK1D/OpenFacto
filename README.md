# OpenFacto

OpenFacto is a C++/raylib 2D tile-based survival + automation prototype inspired by Factorio.

This drop implements **Milestone 1 + Milestone 2**:
- Fixed-tick simulation (20 TPS) and render loop (60 FPS target)
- Procedural chunk-like world generation with deterministic seed
- Smooth WASD movement + collision with water
- Camera follow + zoom
- HUD, debug overlay, minimap toggle, inventory panel, hotbar
- Hold-left-click mining on resource tiles and item collection into inventory

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

## Controls
- `WASD`: move
- Mouse wheel: zoom
- `Left Mouse` (hold): mine selected resource tile
- `1..9`: hotbar slot select
- `TAB`: inventory
- `M`: minimap toggle
- `` ` ``: debug overlay toggle
- `ESC`: close window

## Troubleshooting
- If raylib download fails, verify internet access and rerun configure.
- If executable cannot find graphics context in headless environments, run locally with desktop GPU/display.
- If CMake cache gets corrupted after toolchain changes, delete `build/` and reconfigure.

## Architecture
- `src/game`: game state + loop
- `src/core`: fixed time + input abstraction
- `src/world`: procedural generation + tile/resource simulation
- `src/render`: world and effects rendering
- `src/ui`: HUD and menu overlays
- `src/data`: item/recipe/tech definitions and balance constants
- `src/systems`: milestone placeholders for automation/combat/power progression

## TODO Roadmap (Milestones 3-8)
- Milestone 3: hand crafting + furnace smelting
- Milestone 4: building placement + chest inventories
- Milestone 5: belts + inserters
- Milestone 6: power network + powered drills + tech tree research progression
- Milestone 7: enemy nests, pathing, waves, defenses
- Milestone 8: expanded guide/objectives, balancing pass, optional save/load

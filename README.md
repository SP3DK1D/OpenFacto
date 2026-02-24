# OpenFacto

OpenFacto is a C++/raylib 2D tile-based survival + automation prototype inspired by Factorio.

## Quick Launch (Top Priority)

### Windows
- Double-click `launch.bat`
- Or run in terminal:
  ```bat
  launch.bat
  launch.bat Release
  ```

### Linux/macOS
```bash
chmod +x launch.sh
./launch.sh
./launch.sh Release
```

Launcher behavior:
- detects and creates `build/` automatically
- prefers Ninja when available
- configures + builds Debug by default (Release via argument)
- runs executable from project root, with relative paths

## Manual Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/bin/openfacto
```

## Milestone 7 Implemented (Enemies + Nests + Defense)
- Nests spawn outside spawn-safe area and create enemies over time.
- Threat scales from pollution + progression.
- Enemy behavior: wander/chase-like movement, obstacle sidestep fallback.
- Player melee attack (`Space`) with hit flash.
- Player death + respawn penalty.
- Defensive buildings: Wall + Turret.
- Turret consumes ammo and auto-targets nearby enemies.

### Milestone 7 controls
- `Space`: melee attack
- `F` + hold mining on building: deconstruct
- Place `Wall`/`Turret` from hotbar after crafting

### Milestone 7 smoke tests
- Build and run with launcher.
- Mine resources, craft ammo/turret/walls.
- Wait for nest spawns; verify enemies chase/attack.
- Verify turret consumes ammo and kills enemies.
- Verify death respawn penalty behavior.

## Milestone 8 Implemented (Guide + Objectives + Save/Load + QoL)
- Full-screen guide (`G`) with controls and progression explanations.
- Objectives checklist + next objective tracker + completion notifications.
- Save/Load system (`F5` save, `F9` load) with seed/player/inventory/buildings/nests/stats.
- Autosave (`autosave.ofs`) and configurable settings via `config.ini`.
- Recipe search in inventory/crafting panel.
- Minimap now shows pollution/nest summary.

### Milestone 8 controls
- `G`: guide toggle
- `F5`: manual save to `savegame.ofs`
- `F9`: manual load from `savegame.ofs`
- `TAB`: open inventory and type to filter recipes

### Milestone 8 smoke tests
- Craft and place multiple machine types.
- Save with `F5`, alter world, load with `F9`, confirm restoration.
- Wait for autosave and verify `autosave.ofs` appears.
- Open guide and objective list display.

## config.ini
Optional file in repo root:
```ini
autosave=1
sim_speed=1.0
```

## Known Limitations / TODO
- Limited-radius obstacle fallback is used instead of full A* pathfinding.
- Pollution diffusion is global scalar, not per-chunk heat simulation.
- Turret ammo source is player inventory (not local chest logistics).
- Wave scripting and advanced enemy archetypes are pending.

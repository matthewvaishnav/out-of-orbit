# Out of Orbit

A C++ arcade shoot-em-up built with Raylib. Features a vaporwave aesthetic, multiple ship types, and a wave-based enemy system.

## Project Structure
The codebase is modular, split across several files for easier management:
- `main.cpp`: Entry point and main game loop.
- `game.cpp`: Core game logic and state management.
- `world.cpp`: Level data, enemy spawning, and wave logic.
- `hud.cpp`: UI rendering and status bars.
- `draw.cpp`: Graphics and sprite rendering.
- `vfx.cpp`: Particle systems and visual effects.
- `data.cpp`: Static game data and constants.

## Features
- **Object Pooling:** Used for bullets (`BULLET_POOL_SIZE = 1024`) to avoid runtime allocations and keep performance stable.
- **Raylib Integration:** All rendering and input is handled via Raylib.
- **Ship Types:** Four distinct ships (Interceptor, Brawler, Phantom, Titan) with different stats.
- **Weapon System:** Three prayer types (Fire, Ice, Spirit) that change bullet behavior.

## Building
Requires `raylib` installed.
·
### Using the script:
```bash
./build.sh
```

### Using Make:
```bash
make
```

## Controls
- **WASD / Arrows**: Move
- **Space / Click**: Shoot
- **Esc**: Pause

## License
MIT

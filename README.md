# Out of Orbit: Neon Miko — Vaporwave Shrine Edition

A high-performance, retro-infused Vaporwave bullet-hell odyssey written in C++ using Raylib.

## The Mission: Beyond the Atmosphere
**Out of Orbit** is a high-octane, pixel-perfect "shmup" (shoot-em-up) where the rhythm of the synthwave soundtrack is matched only by the intensity of the bullet patterns.

Step into the cockpit of a shrine-powered interceptor and blast through a kaleidoscopic dreamscape. This isn't just a battle for survival—it's a journey through a neon-soaked void filled with:
*   **Vaporwave Aesthetics:** A hand-crafted "Synthwave Palette" that makes every frame feel like a 1980s retro-future fever dream.
*   **Diverse Ship Archetypes:** Choose your path—from the agile **Interceptor** to the heavy-hitting **Titan** or the elusive **Phantom.**
*   **Shrine Powers:** Master the three sacred Pray Types: **Fire**, **Ice**, and **Spirit** to turn the tide of battle.
*   **Relentless Waves:** Face off against E_BASIC drones, tanky E_VOID behemoths, and screen-filling bosses that pulse with the energy of a dying sun.

## Technical Prowess (Under the Hood)
For the CST-minded explorers, here’s how we keep the frames buttery smooth even when the screen is full of chaos:
*   **Zero Fragmentation:** Implements a custom **Object Pooling system** for bullets (`BULLET_POOL_SIZE = 1024`). No heap allocations mid-frame means zero stutters, ever.
*   **Modular Architecture:** A clean, decoupled C++ design across specialized modules (`vfx.cpp`, `world.cpp`, `draw.cpp`, `hud.cpp`) for maximum maintainability.
*   **Raylib Core:** Leveraging the power of [Raylib](https://www.raylib.com/) for low-level, high-performance rendering and input handling.

## Building the Game

### Prerequisites
*   **C++ Compiler** (GCC/Clang recommended)
*   **[Raylib](https://github.com/raysan5/raylib)** installed on your system.

### Build Instructions
Just hit the ignition:
```bash
./build.sh
# or
make
```

## Controls
*   **WASD / Arrow Keys:** Navigate the void.
*   **Space / Left Click:** Unleash neon fire.
*   **Esc:** Pause the dream.

## License
This project is licensed under the MIT License — feel free to fork, mod, and make it your own!

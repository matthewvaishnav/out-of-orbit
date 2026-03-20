@echo off
REM Performance Optimization Patch for Out of Orbit
REM Run this script in your project directory

echo Applying optimizations...

REM 1. Update void_blaster.h - Add PARTICLE_LIMIT
powershell -Command "(Get-Content void_blaster.h) -replace '// Bullet pool sizes.*', '// Bullet pool sizes -- pre-allocated, no heap fragmentation mid-frame`nconstexpr int BULLET_POOL_SIZE      = 1024;`nconstexpr int BOSS_BULLET_POOL_SIZE = 512;`n`n// Particle system — increased from 400 for better explosion density`nconstexpr int PARTICLE_LIMIT = 800;' | Set-Content void_blaster.h"

REM 2. Update vfx.cpp - Change particle limit
powershell -Command "(Get-Content vfx.cpp) -replace 'g\.particles\.size\(\)\>=400', 'g.particles.size()>=PARTICLE_LIMIT' | Set-Content vfx.cpp"

REM 3. Update game.cpp - Add vector pre-allocation
powershell -Command "(Get-Content game.cpp) -replace 'void InitGame\(Game& g\)\{', 'void InitGame(Game& g){' -replace 'int hs=g\.highScore; int ss=g\.selectedShip;', 'int hs=g.highScore; int ss=g.selectedShip;`n    // Pre-allocate vectors to avoid mid-game reallocations`n    g.enemies.reserve(64);`n    g.particles.reserve(PARTICLE_LIMIT);`n    g.pickups.reserve(32);`n    g.popups.reserve(16);`n    g.shootStars.reserve(16);`n    int hs=g.highScore; int ss=g.selectedShip;' | Set-Content game.cpp"

echo Done! Rebuild your project to see the performance improvements.
pause

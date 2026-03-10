#!/bin/bash
# Run this from the same folder as your .cpp files
# It recompiles everything and replaces void_blaster

set -e
cd "$(dirname "$0")"

echo "Compiling..."
g++ -std=c++17 -O3 -march=native -mtune=native -flto -funroll-loops \
    -fomit-frame-pointer -ffast-math -Wall -Wno-unused-parameter \
    main.cpp data.cpp game.cpp draw.cpp hud.cpp vfx.cpp world.cpp \
    -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -flto \
    -o void_blaster

echo "Done. Run ./void_blaster"

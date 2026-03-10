#!/bin/bash
# Run this from the same folder as your .cpp files
# It recompiles everything and replaces out_of_orbit

set -e
cd "$(dirname "$0")"

echo "Compiling..."
g++ -std=c++17 -O3 -march=native -mtune=native -flto -funroll-loops \
    -fomit-frame-pointer -ffast-math -Wall -Wno-unused-parameter \
    main.cpp data.cpp game.cpp draw.cpp hud.cpp vfx.cpp world.cpp \
    -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -flto \
    -o out_of_orbit

echo "Done. Run ./out_of_orbit"

CXX     = g++
CXXFLAGS= -std=c++17 -O3 -march=native -mtune=native -flto -funroll-loops -fomit-frame-pointer -ffast-math -Wall -Wno-unused-parameter
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -flto

SRCS    = main.cpp data.cpp game.cpp draw.cpp hud.cpp vfx.cpp world.cpp
TARGET  = out_of_orbit

$(TARGET): $(SRCS) void_blaster.h
	$(CXX) $(CXXFLAGS) $(SRCS) $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET)

#pragma once
#include "grid.hpp"
#include <optional>

Grid generateRandomGrid(int rows, int cols, double wallDensity = 0.3, unsigned int seed = 0);
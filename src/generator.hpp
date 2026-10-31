#pragma once
#include "grid.hpp"
#include <optional>
#include <random>

int frontierGrowth(Grid& grid, std::mt19937& rng, double initialProbability, double decayRate, CellType terrainType);
Grid generateRandomGrid(int rows, int cols, double wallDensity = 0.3, double mudDensity = 0.1, double waterDensity = 0.3, unsigned int seed = 0);
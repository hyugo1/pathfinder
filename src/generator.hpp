#pragma once
#include "grid.hpp"
#include <optional>
#include <random>

void frontierGrowth(Grid& grid, std::mt19937& rng, double initialProbability, double decayRate, CellType terrainType);
Grid generateRandomGrid(int rows, int cols, double wallDensity = 0.3, double mudCount = 10.0, double waterCount = 10.0, unsigned int seed = 0);
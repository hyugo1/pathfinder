#pragma once
 
#include <optional>
#include <string>
#include <vector>
#include <variant>
#include "algorithms.hpp"
#include "appstate.hpp"
#include "input.hpp"

enum class InputLabel {
    None,
    Seed,
    WallDensity,
    MudBlobDensity,
    WaterBlobDensity,
};

using ParsedInput = std::variant<unsigned int, double>;

const double minWallDensityRange = 0.0;
const double maxWallDensityRange = 100.0;
const double minBlobDensityRange = 0.0;
const double maxBlobDensityRange = 100.0;

struct GenerationSettings {
    std::optional<unsigned int> seed = 0;
    double wallDensity = 30.0;
    double mudCount    = 10.0;
    double waterCount  = 10.0;
};

std::optional<double> parseDouble(const std::string& input, double min, double max);
unsigned int generateRandomSeed();
std::optional<unsigned int> parseSeed(const std::string& input);
std::optional<ParsedInput> parseInput(InputLabel inputlabel, const std::string& inputstring);
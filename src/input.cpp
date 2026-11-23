#include "input.hpp"
#include <optional>
#include <random>
#include <iostream>
#include <limits>
#include <cmath>
#include <variant>
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include <string>

std::optional<double> parseDouble(const std::string& input, double min, double max) {
    try {
        std::size_t pos = 0;
        double value = std::stod(input, &pos);
    
        if (pos != input.size() || !std::isfinite(value)) {
            throw std::invalid_argument("Invalid number");
        }

        if (value < min || value > max) {
            throw std::invalid_argument("Value outside valid range");
        }

        return value;

    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid input: " << e.what()
                  << ". Expected a value between "
                  << min << " and " << max << ".\n";
        return std::nullopt;

    } catch (const std::out_of_range& e) {
        std::cerr << "Input value is out of range.\n";
        return std::nullopt;
    }
}

unsigned int generateRandomSeed() {
    std::random_device rd;
    unsigned int seed = rd();
    std::cout << "Using random seed: " << seed << '\n';
    return seed;
}

std::optional<unsigned int> parseSeed(const std::string& input) {
    try {
        if (input.find_first_not_of("0123456789") != std::string::npos) {
            throw std::invalid_argument("Seed must contain digits only");
        }
        std::size_t pos = 0;
        unsigned long value = std::stoul(input, &pos);

        if (pos != input.size() ||
            value > std::numeric_limits<unsigned int>::max()) {
            throw std::out_of_range("Seed value is out of range");
        }

        return static_cast<unsigned int>(value);

    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid seed: " << e.what() << '\n';
        return std::nullopt;

    } catch (const std::out_of_range& e) {
        std::cerr << "Seed value is out of range.\n";
        return std::nullopt;
    }
}

std::optional<ParsedInput> parseInput(InputLabel inputlabel, const std::string& inputstring) {
    if (inputstring.empty()) {
        switch (inputlabel) {
            case InputLabel::Seed:
                return ParsedInput{generateRandomSeed()};

            case InputLabel::WallDensity:
                return ParsedInput{GenerationSettings{}.wallDensity};

            case InputLabel::WaterBlobDensity:
                return ParsedInput{GenerationSettings{}.waterCount};

            case InputLabel::MudBlobDensity:
                return ParsedInput{GenerationSettings{}.mudCount};

            case InputLabel::None:
                return std::nullopt;
        }
    }
    switch (inputlabel) {
        case (InputLabel::Seed): {
            auto value = parseSeed(inputstring);
            if (!value) return std::nullopt;
            return ParsedInput{*value};
        }
        case (InputLabel::WallDensity): {
            auto value = parseDouble(inputstring, minWallDensityRange, maxWallDensityRange);
            if (!value) return std::nullopt;
            return ParsedInput{*value};
        }
        case InputLabel::WaterBlobDensity:
        case InputLabel::MudBlobDensity: {
            auto value = parseDouble(inputstring, minBlobDensityRange, maxBlobDensityRange);
            if (!value) return std::nullopt;
            return ParsedInput{*value};
        }
        case InputLabel::None:
            return std::nullopt;
    }
    return std::nullopt;
}

std::string trim(const std::string& input) {
    auto start = std::find_if_not(input.begin(), input.end(),
        [](unsigned char ch) {
            return std::isspace(ch);
        });

    auto end = std::find_if_not(input.rbegin(), input.rend(),
        [](unsigned char ch) {
            return std::isspace(ch);
        }).base();

    if (start >= end) {
        return "";
    }

    return std::string(start, end);
}
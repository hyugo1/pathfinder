#include "grid.hpp"

#include <gtest/gtest.h>

// TEST(SuiteName, TestName)
TEST(GridTest, EmptyGrid) {
    Grid grid = {};
    EXPECT_TRUE(grid.empty());
    EXPECT_FALSE(validateExactlyOneStartAndEnd(grid));
}

TEST(GridTest, StartDoesNotExist) {
    Grid grid = {
        {CellType::Open, CellType::Open, CellType::Wall},
        {CellType::Wall, CellType::Wall, CellType::End},
        {CellType::Open, CellType::Wall, CellType::Open}
    };
    EXPECT_FALSE(validateExactlyOneStartAndEnd(grid));
}

TEST(GridTest, MoreThanOneStart) {
    Grid grid = {
        {CellType::Start, CellType::Start, CellType::Wall},
        {CellType::Open, CellType::Open, CellType::End},
        {CellType::Open, CellType::Wall, CellType::Open}
    };
    EXPECT_FALSE(validateExactlyOneStartAndEnd(grid));
}

TEST(GridTest, ExactlyOneStartAndEnd) {
    Grid grid = {
        {CellType::Start, CellType::Open, CellType::Wall},
        {CellType::Open, CellType::Open, CellType::End},
        {CellType::Open, CellType::Wall, CellType::Open}
    };
    EXPECT_TRUE(validateExactlyOneStartAndEnd(grid));
}

TEST(GridTest, ResetGrid) {
    Grid grid = {
        {CellType::Start, CellType::Open, CellType::Wall},
        {CellType::Open, CellType::Open, CellType::End},
        {CellType::Open, CellType::Wall, CellType::Open}
    };
    resetGrid(grid);
    EXPECT_FALSE(validateExactlyOneStartAndEnd(grid));
    for (int i{0}; i < grid.size(); ++i) {
        for (int j{0}; j < grid[0].size(); ++j) {
            EXPECT_EQ(grid[i][j], CellType::Open);
        }
    }
}
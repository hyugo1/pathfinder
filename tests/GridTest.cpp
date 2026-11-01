#include "grid.hpp"

#include <gtest/gtest.h>

class GridFixture : public ::testing::Test {
protected:
    Grid grid;

    void SetUp() override {
        // S*#
        // #*E
        // ...
        grid = {
            {CellType::Start, CellType::Open, CellType::Wall},
            {CellType::Open,  CellType::Open, CellType::End},
            {CellType::Open,  CellType::Open, CellType::Open}
        };
    }
};

// TEST(SuiteName, TestName)
TEST(GridTest, EmptyGrid) {
    Grid grid = {};
    EXPECT_TRUE(grid.empty());
    EXPECT_FALSE(validateExactlyOneStartAndEnd(grid));
}

TEST_F(GridFixture, StartDoesNotExist) {
    grid[0][0] = CellType::Open;
    EXPECT_FALSE(validateExactlyOneStartAndEnd(grid));
}

TEST_F(GridFixture, MoreThanOneStart) {
    grid[0][1] = CellType::Start;
    EXPECT_FALSE(validateExactlyOneStartAndEnd(grid));
}

TEST_F(GridFixture, EndDoesNotExist) {
    grid[1][2] = CellType::Open;
    EXPECT_FALSE(validateExactlyOneStartAndEnd(grid));
}

TEST_F(GridFixture, MoreThanOneEnd) {
    grid[2][2] = CellType::End;
    EXPECT_FALSE(validateExactlyOneStartAndEnd(grid));
}

TEST_F(GridFixture, ExactlyOneStartAndEnd) {
    EXPECT_TRUE(validateExactlyOneStartAndEnd(grid));
}

TEST_F(GridFixture, TestIsValidGrid) {
    EXPECT_TRUE(validateGrid(grid));
}

TEST_F(GridFixture, IsValidGrid) {
    EXPECT_TRUE(validateGrid(grid));
}

TEST_F(GridFixture, StartDoesNotExistInIsValidGrid) {
    grid[0][0] = CellType::Open;
    EXPECT_FALSE(validateGrid(grid));
}

TEST_F(GridFixture, MultipleStartsExistInIsValidGrid) {
    grid[2][2] = CellType::Start;
    EXPECT_FALSE(validateGrid(grid));
}

TEST_F(GridFixture, ResetGrid) {
    resetGrid(grid);
    EXPECT_FALSE(validateExactlyOneStartAndEnd(grid));
    for (const auto& row : grid) {
        for (CellType cell : row) {
            EXPECT_EQ(cell, CellType::Open);
        }
    }
}
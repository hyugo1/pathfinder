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

// TEST(SuiteName, MethodName_StateUnderTest_ExpectedBehavior)
TEST(GridTest, ValidateStartEnd_EmptyGrid_ReturnsFalse) {
    Grid grid = {};
    EXPECT_FALSE(validateExactlyOneStartAndEnd(grid));
}

TEST_F(GridFixture, ValidateStartEnd_NoStart_ReturnsFalse) {
    grid[0][0] = CellType::Open;
    EXPECT_FALSE(validateExactlyOneStartAndEnd(grid));
}

TEST_F(GridFixture, ValidateStartEnd_MoreThanOneStart_ReturnsFalse) {
    grid[0][1] = CellType::Start;
    EXPECT_FALSE(validateExactlyOneStartAndEnd(grid));
}

TEST_F(GridFixture, ValidateStartEnd_NoEnd_ReturnsFalse) {
    grid[1][2] = CellType::Open;
    EXPECT_FALSE(validateExactlyOneStartAndEnd(grid));
}

TEST_F(GridFixture, ValidateStartEnd_MoreThanOneEnd_ReturnsFalse) {
    grid[2][2] = CellType::End;
    EXPECT_FALSE(validateExactlyOneStartAndEnd(grid));
}

TEST_F(GridFixture, ValidateStartEnd_OneEach_ReturnsTrue) {
    EXPECT_TRUE(validateExactlyOneStartAndEnd(grid));
}

TEST(GridTest, ValidateGrid_EmptyGrid_ReturnsFalse) {
    Grid grid{};
    EXPECT_FALSE(validateGrid(grid));
}

TEST_F(GridFixture, ValidateGrid_ValidGrid_ReturnsTrue) {
    EXPECT_TRUE(validateGrid(grid));
}

TEST_F(GridFixture, ValidateGrid_NoStart_ReturnsFalse) {
    grid[0][0] = CellType::Open;
    EXPECT_FALSE(validateGrid(grid));
}

TEST_F(GridFixture, ValidateGrid_MoreThanOneStart_ReturnsFalse) {
    grid[2][2] = CellType::Start;
    EXPECT_FALSE(validateGrid(grid));
}

TEST_F(GridFixture, ResetGrid_AnyGrid_SetsEveryCellOpen) {
    resetGrid(grid);
    for (const auto& row : grid) {
        for (CellType cell : row) {
            EXPECT_EQ(cell, CellType::Open);
        }
    }
}

TEST_F(GridFixture, IsInBounds_CoordinatesWithinGridEdges_ReturnsTrue) {
    const int rows = static_cast<int>(grid.size());
    const int cols = static_cast<int>(grid[0].size());

    EXPECT_TRUE(isInBounds(grid, 0, 0));
    EXPECT_TRUE(isInBounds(grid, rows - 1, cols - 1));
}

TEST_F(GridFixture, IsInBounds_CoordinatesOutsideGridEdges_ReturnsFalse) {
    const int rows = static_cast<int>(grid.size());
    const int cols = static_cast<int>(grid[0].size());

    EXPECT_FALSE(isInBounds(grid, -1, -1));
    EXPECT_FALSE(isInBounds(grid, rows - 1, -1));
    EXPECT_FALSE(isInBounds(grid, -1, cols - 1));
    EXPECT_FALSE(isInBounds(grid, rows, cols));
}

TEST(GridTest, IsInBounds_NonSquareGridAllWithinBounds_ReturnsTrue) {
    Grid grid = {
        {CellType::Start, CellType::Open, CellType::Wall},
        {CellType::Open,  CellType::Open, CellType::End},
        {CellType::Open,  CellType::Open, CellType::Open},
        {CellType::Open,  CellType::Open, CellType::Open},
        {CellType::Open,  CellType::Open, CellType::Open}
    };
    const int rows = static_cast<int>(grid.size());
    const int cols = static_cast<int>(grid[0].size());

    for (int i {0}; i < rows; ++i) {
        for (int j {0}; j < cols; ++j) {
            EXPECT_TRUE(isInBounds(grid, i, j));
        }
    }
}

TEST_F(GridFixture, IsTraversable_CoordinatesWithinGridEdges_ReturnsTrue) {
    EXPECT_TRUE(isTraversable(grid, 0, 0));
    EXPECT_TRUE(isTraversable(grid, 1, 2));//end
}

TEST_F(GridFixture, IsTraversable_CoordinatesOutsideGridEdges_ReturnsFalse) {
    const int rows = static_cast<int>(grid.size());
    const int cols = static_cast<int>(grid[0].size());
    
    EXPECT_FALSE(isTraversable(grid, rows, cols));//out of bounds
}

TEST_F(GridFixture, IsTraversable_WallCell_ReturnsFalse) {
    EXPECT_FALSE(isTraversable(grid, 0, 2));//wall
}
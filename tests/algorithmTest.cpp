#include "algorithms.hpp"
#include "grid.hpp"
#include <iostream>

#include <gtest/gtest.h>

class AlgorithmFixture : public ::testing::Test {
protected:
    Grid grid;

    void SetUp() override {
        // S*#
        // #*E
        // ...
        grid = {
            {CellType::Start, CellType::Open, CellType::Wall},
            {CellType::Wall,  CellType::Open, CellType::Open},
            {CellType::Open,  CellType::Open, CellType::End}
        };
    }
};

// TEST(SuiteName, MethodName_StateUnderTest_ExpectedBehavior)
TEST(AlgorithmTest, BFS_EmptyGrid_ReturnsEmptyResult) {
    Grid grid = {};
    Position start{0, 0};
    Position end{0, 0};
    
    SearchResult result = breadthFirstSearch(grid, start, end);
    EXPECT_TRUE(result.path.empty());
    EXPECT_TRUE(result.explorationOrder.empty());
    EXPECT_EQ(result.totalCost, 0);
}

TEST_F(AlgorithmFixture, BFS_StartOutOfBounds_ReturnsEmptySearchResult) {
    Position start = {-1, 0};
    Position end = {1, 2};
    
    SearchResult result = breadthFirstSearch(grid, start, end);
    EXPECT_TRUE(result.path.empty());
    EXPECT_TRUE(result.explorationOrder.empty());
}

TEST_F(AlgorithmFixture, BFS_TestNoPath_ReturnsNoPath) {
    grid[2][1] = CellType::Wall;
    grid[1][2] = CellType::Wall;
    Position start = {0, 0};
    Position end = {2, 2};
    
    SearchResult result = breadthFirstSearch(grid, start, end);

    EXPECT_TRUE(result.path.empty());
    EXPECT_FALSE(result.explorationOrder.empty());
    EXPECT_EQ(result.explorationOrder.front(), start);

    std::vector<Position> expected = {
        {0, 0}, {0, 1}, {1, 1}
    };
    EXPECT_EQ(result.explorationOrder, expected);
        
    auto it = std::find(result.explorationOrder.begin(), result.explorationOrder.end(), end);//returns result.exploartion.end() if it doesnt find "end" position
    EXPECT_EQ(it, result.explorationOrder.end());
}

TEST_F(AlgorithmFixture, BFS_CheckOrder_ReturnsSpecificOrder) {
    Position start = {0, 0};
    Position end = {1, 2};
    
    SearchResult result = breadthFirstSearch(grid, start, end);
    
    //up, down left, right is the order
    EXPECT_TRUE(result.explorationOrder.front() == start);
    std::vector<Position> expected_path = {{0, 0}, {0, 1}, {1, 1}, {1,2}};

    EXPECT_EQ(result.path, expected_path);

    EXPECT_FALSE(result.explorationOrder.empty());
    EXPECT_EQ(result.explorationOrder.front(), start);
}

TEST(AlgorithmTest, BFS_WindingPathAroundWalls_ReturnsExactPath) {
    Position start{0, 0};
    Position end{4, 2};
    
    Grid grid = {
            {CellType::Start, CellType::Open, CellType::Wall},
            {CellType::Wall,  CellType::Open, CellType::Wall},
            {CellType::Open,  CellType::Open, CellType::Wall},
            {CellType::Open,  CellType::Wall, CellType::Wall},
            {CellType::Open,  CellType::Open, CellType::End}
    };
    SearchResult result = breadthFirstSearch(grid, start, end);
    
    std::vector<Position> expected_path = {
        {0, 0}, {0, 1}, {1, 1}, {2, 1}, {2, 0}, {3, 0}, {4, 0}, {4, 1}, {4, 2}, 
    };
    EXPECT_EQ(result.path, expected_path);
}
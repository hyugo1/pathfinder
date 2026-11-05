#include "algorithms.hpp"
#include "grid.hpp"
#include <iostream>

#include <gtest/gtest.h>

class AlgorithmFixture : public ::testing::Test {
protected:
    Grid grid;

    void SetUp() override {
        // S.#
        // #..
        // ..E
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
    Position end = {2, 2};
    
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
    Position end = {2, 2};
    
    SearchResult result = breadthFirstSearch(grid, start, end);
    
    //up, down left, right is the order
    EXPECT_TRUE(result.explorationOrder.front() == start);
    std::vector<Position> expected_path = {{0, 0}, {0, 1}, {1, 1}, {2,1}, {2, 2}};
    EXPECT_EQ(result.path, expected_path);

    std::vector<Position> expected_explorationOrder = {{0, 0}, {0, 1}, {1, 1}, {2, 1}, {1, 2}, {2, 0}, {2, 2}};//explore is done only when the end node is popped
    EXPECT_EQ(result.explorationOrder, expected_explorationOrder);

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


TEST(AlgorithmTest, BFS_AddingObstacleCosts_ReturnsNonOptimalPath) {
    Position start{0, 0};
    Position end{4, 2};
    
    Grid grid = {
            {CellType::Start, CellType::Open, CellType::Water},
            {CellType::Water, CellType::Open, CellType::Water},
            {CellType::Open,  CellType::Open, CellType::Water},
            {CellType::Open,  CellType::Water,CellType::Water},
            {CellType::Open,  CellType::Open, CellType::End}
    };
    SearchResult result = breadthFirstSearch(grid, start, end);

    std::vector<Position> expectedPath = {
        {0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {4, 1}, {4, 2}
    };
    EXPECT_EQ(result.path, expectedPath);

    std::vector<Position> bestCostEfficientPath = {
        {0, 0}, {0, 1}, {1, 1}, {2, 1}, {2, 0}, {3, 0}, {4, 0}, {4, 1}, {4, 2}
    };
    EXPECT_GT(result.totalCost, calculatePathCost(grid,  bestCostEfficientPath));
}


// ---- DFS ----

// TEST(SuiteName, MethodName_StateUnderTest_ExpectedBehavior)
TEST(AlgorithmTest, DFS_EmptyGrid_ReturnsEmptyResult) {
    Grid grid = {};
    Position start{0, 0};
    Position end{0, 0};
    
    SearchResult result = depthFirstSearch(grid, start, end);
    EXPECT_TRUE(result.path.empty());
    EXPECT_TRUE(result.explorationOrder.empty());
    EXPECT_EQ(result.totalCost, 0);
}

TEST_F(AlgorithmFixture, DFS_StartOutOfBounds_ReturnsEmptySearchResult) {
    Position start = {-1, 0};
    Position end = {2, 2};
    
    SearchResult result = depthFirstSearch(grid, start, end);
    EXPECT_TRUE(result.path.empty());
    EXPECT_TRUE(result.explorationOrder.empty());
}


TEST_F(AlgorithmFixture, DFS_TestNoPath_ReturnsNoPath) {
    grid[2][1] = CellType::Wall;
    grid[1][2] = CellType::Wall;
    Position start = {0, 0};
    Position end = {2, 2};
    
    SearchResult result = depthFirstSearch(grid, start, end);

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

TEST_F(AlgorithmFixture, DFS_CheckOrder_ReturnsSpecificOrder) {
    Position start = {0, 0};
    Position end = {2, 2};
    
    SearchResult result = depthFirstSearch(grid, start, end);
    ASSERT_FALSE(result.explorationOrder.empty());
    EXPECT_EQ(result.explorationOrder.front(), start);
    //adds all valid, and pops the last added one and go from that.
    std::vector<Position> expected_path = {{0, 0}, {0, 1}, {1, 1}, {2, 1}, {2, 2}};
    EXPECT_EQ(result.path, expected_path);
    std::vector<Position> expected_explorationOrder = {{0, 0}, {0, 1}, {1, 1}, {2, 1}, {2, 0}, {2, 2}};
    EXPECT_EQ(result.explorationOrder, expected_explorationOrder);

    EXPECT_FALSE(result.explorationOrder.empty());
    EXPECT_EQ(result.explorationOrder.front(), start);
}

TEST(AlgorithmTest, DFS_WindingPathAroundWalls_ReturnsExactPath) {
    Position start{0, 0};
    Position end{4, 2};
    
    Grid grid = {
            {CellType::Start, CellType::Open, CellType::Wall},
            {CellType::Wall,  CellType::Open, CellType::Wall},
            {CellType::Open,  CellType::Open, CellType::Wall},
            {CellType::Open,  CellType::Wall, CellType::Wall},
            {CellType::Open,  CellType::Open, CellType::End}
    };
    SearchResult result = depthFirstSearch(grid, start, end);
    
    std::vector<Position> expected_path = {
        {0, 0}, {0, 1}, {1, 1}, {2, 1}, {2, 0}, {3, 0}, {4, 0}, {4, 1}, {4, 2}, 
    };
    EXPECT_EQ(result.path, expected_path);
}


TEST(AlgorithmTest, DFS_AddingObstacleCosts_ReturnsNonOptimalPath) {
    Position start{0, 0};
    Position end{4, 2};
    
    Grid grid = {
            {CellType::Start, CellType::Open, CellType::Water},
            {CellType::Water, CellType::Open, CellType::Water},
            {CellType::Open,  CellType::Open, CellType::Water},
            {CellType::Open,  CellType::Water,CellType::Water},
            {CellType::Open,  CellType::Open, CellType::End}
    };
    SearchResult result = depthFirstSearch(grid, start, end);
    
    std::vector<Position> expectedExplorationOrder = {
        {0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {4, 1}, {4, 2}
    };
    EXPECT_EQ(result.explorationOrder, expectedExplorationOrder);

    std::vector<Position> expectedPath = {
        {0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {4, 1}, {4, 2}
    };
    EXPECT_EQ(result.path, expectedPath);

    std::vector<Position> bestCostEfficientPath = {
        {0, 0}, {0, 1}, {1, 1}, {2, 1}, {2, 0}, {3, 0}, {4, 0}, {4, 1}, {4, 2}
    };
    EXPECT_GT(result.totalCost, calculatePathCost(grid, bestCostEfficientPath));
}
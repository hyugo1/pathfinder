#include "console.hpp"
#include "input.hpp"

#include <gtest/gtest.h>

TEST(ConsoleTest, printAlgorithmName_ReturnsCorrectInput) {
    Algorithm algo = Algorithm::DFS;

    testing::internal::CaptureStdout();
    printAlgorithmName(algo);
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_EQ(output, "Using DFS.\n");
}

// --- seed --- 
TEST(ConsoleTest, askForSeed_ReturnsCorrectInput) {
    std::istringstream input("5\n");
    
    // Redirect std::cin
    std::streambuf* oldCin = std::cin.rdbuf(input.rdbuf());
    
    testing::internal::CaptureStdout();
    std::optional<unsigned int> value = askForSeed();
    std::string output = testing::internal::GetCapturedStdout();

    // Restore std::cin
    std::cin.rdbuf(oldCin);
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value, std::optional<unsigned int>(5));

    EXPECT_NE(output.find("Enter a seed"), std::string::npos);
}


TEST(ConsoleTest, askForSeed_EmptyInput_ReturnsRandomSeed) {
    std::istringstream input("\n");
    std::streambuf* oldCin = std::cin.rdbuf(input.rdbuf());

    auto value = askForSeed();

    std::cin.rdbuf(oldCin);

    ASSERT_TRUE(value.has_value());
    EXPECT_GT(value.value(), 0u);
}

TEST(ConsoleTest, askForSeed_InvalidInputThenValidInput) {
    std::istringstream input("abc\n-5\n10\n");
    std::streambuf* oldCin = std::cin.rdbuf(input.rdbuf());

    auto value = askForSeed();

    std::cin.rdbuf(oldCin);

    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value.value(), 10u);
}

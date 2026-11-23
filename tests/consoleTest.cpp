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


// --- wall density --- 
TEST(ConsoleTest, askForWallDensity_ReturnsCorrectInput) {
    std::istringstream input("4\n");
    
    std::streambuf* oldCin = std::cin.rdbuf(input.rdbuf());
    
    testing::internal::CaptureStdout();
    std::optional<double> value = askForWallDensity();
    std::string output = testing::internal::GetCapturedStdout();

    std::cin.rdbuf(oldCin);
    ASSERT_TRUE(value.has_value());
    EXPECT_DOUBLE_EQ(value.value(), 4.0);
}

TEST(ConsoleTest, askForWallDensity_EmptyInput_ReturnsDefaultWallDensity) {
    std::istringstream input("\n");
    std::streambuf* oldCin = std::cin.rdbuf(input.rdbuf());

    auto value = askForWallDensity();

    std::cin.rdbuf(oldCin);

    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value.value(), GenerationSettings{}.wallDensity);
}

TEST(ConsoleTest, askForWallDensity_InvalidInputThenValidInput) {
    std::istringstream input("abc\n-5\n10\n");
    std::streambuf* oldCin = std::cin.rdbuf(input.rdbuf());

    auto value = askForWallDensity();

    std::cin.rdbuf(oldCin);

    ASSERT_TRUE(value.has_value());
    EXPECT_DOUBLE_EQ(value.value(), 10.0);
}

TEST(ConsoleTest, askForWallDensity_EOF_ReturnsNullopt) {
    std::istringstream input("");
    std::streambuf* oldCin = std::cin.rdbuf(input.rdbuf());

    auto value = askForWallDensity();

    std::cin.rdbuf(oldCin);

    EXPECT_FALSE(value.has_value());
}


TEST(ConsoleTest, askForWallDensity_Zero_ReturnsZero) {
    std::istringstream input("0\n");
    std::streambuf* oldCin = std::cin.rdbuf(input.rdbuf());

    auto value = askForWallDensity();

    std::cin.rdbuf(oldCin);

    ASSERT_TRUE(value.has_value());
    EXPECT_DOUBLE_EQ(value.value(), 0.0);
}

TEST(ConsoleTest, askForWallDensity_MaxValue_Returns100) {
    std::istringstream input("100\n");
    std::streambuf* oldCin = std::cin.rdbuf(input.rdbuf());

    auto value = askForWallDensity();

    std::cin.rdbuf(oldCin);

    ASSERT_TRUE(value.has_value());
    EXPECT_DOUBLE_EQ(value.value(), 100.0);
}

TEST(ConsoleTest, askForWallDensity_InvalidRange_AsksAgain) {
    std::istringstream input("110\n50\n");
    std::streambuf* oldCin = std::cin.rdbuf(input.rdbuf());

    testing::internal::CaptureStderr();

    auto value = askForWallDensity();

    std::string errorOutput = testing::internal::GetCapturedStderr();

    std::cin.rdbuf(oldCin);

    ASSERT_TRUE(value.has_value());
    EXPECT_DOUBLE_EQ(value.value(), 50.0);

    EXPECT_NE(
        errorOutput.find("Invalid input: Value outside valid range."),
        std::string::npos
    );
}
#include "console.hpp"
#include "input.hpp"

#include <gtest/gtest.h>

class CinRedirect {
public:
    // Constructor: temporarily redirects std::cin
    explicit CinRedirect(std::istream& input)
        : oldBuffer(std::cin.rdbuf(input.rdbuf())) {}//input.rdbuf() gives the actual input content. std::cin.rdbuf replaces the buffer used by std::cin

    // Destructor: restores the original std::cin
    ~CinRedirect() {
        std::cin.rdbuf(oldBuffer);
    }

private:
    // Remembers the original std::cin buffer
    std::streambuf* oldBuffer;
};

TEST(ConsoleTest, printAlgorithmName_ReturnsCorrectInput) {
    Algorithm algo = Algorithm::DFS;

    testing::internal::CaptureStdout();
    printAlgorithmName(algo);
    std::string output = testing::internal::GetCapturedStdout();//capture the output instead of displaying it in the terminal

    EXPECT_EQ(output, "Using DFS.\n");
}

// --- seed --- 
TEST(ConsoleTest, askForSeed_ReturnsCorrectInput) {
    std::istringstream input("5\n");
    
    CinRedirect redirect(input);
    testing::internal::CaptureStdout();
    auto value = askForSeed();

    std::string output = testing::internal::GetCapturedStdout();

    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value, std::optional<unsigned int>(5));

    EXPECT_NE(output.find("Enter a seed"), std::string::npos);
}


TEST(ConsoleTest, askForSeed_EmptyInput_ReturnsRandomSeed) {
    std::istringstream input("\n");
    CinRedirect redirect(input);

    auto value = askForSeed();

    ASSERT_TRUE(value.has_value());
    EXPECT_GT(value.value(), 0u);
}

TEST(ConsoleTest, askForSeed_InvalidInputThenValidInput) {
    std::istringstream input("abc\n-5\n10\n");
    CinRedirect redirect(input);

    auto value = askForSeed();

    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value.value(), 10u);
}


// --- wall density --- 
TEST(ConsoleTest, askForWallDensity_ReturnsCorrectInput) {
    std::istringstream input("4\n");
    
    CinRedirect redirect(input);
    
    testing::internal::CaptureStdout();
    std::optional<double> value = askForWallDensity();
    std::string output = testing::internal::GetCapturedStdout();

    ASSERT_TRUE(value.has_value());
    EXPECT_DOUBLE_EQ(value.value(), 4.0);
}

TEST(ConsoleTest, askForWallDensity_EmptyInput_ReturnsDefaultWallDensity) {
    std::istringstream input("\n");
    CinRedirect redirect(input);

    auto value = askForWallDensity();

    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value.value(), GenerationSettings{}.wallDensity);
}

TEST(ConsoleTest, askForWallDensity_InvalidInputThenValidInput) {
    std::istringstream input("abc\n-5\n10\n");
    CinRedirect redirect(input);

    auto value = askForWallDensity();

    ASSERT_TRUE(value.has_value());
    EXPECT_DOUBLE_EQ(value.value(), 10.0);
}

TEST(ConsoleTest, askForWallDensity_EOF_ReturnsNullopt) {
    std::istringstream input("");
    CinRedirect redirect(input);

    auto value = askForWallDensity();

    EXPECT_FALSE(value.has_value());
}


TEST(ConsoleTest, askForWallDensity_Zero_ReturnsZero) {
    std::istringstream input("0\n");
    CinRedirect redirect(input);

    auto value = askForWallDensity();

    ASSERT_TRUE(value.has_value());
    EXPECT_DOUBLE_EQ(value.value(), minWallDensityRange);
}

TEST(ConsoleTest, askForWallDensity_MaxValue_Returns100) {
    std::istringstream input("100\n");
    CinRedirect redirect(input);

    auto value = askForWallDensity();

    ASSERT_TRUE(value.has_value());
    EXPECT_DOUBLE_EQ(value.value(), maxWallDensityRange);
}

TEST(ConsoleTest, askForWallDensity_InvalidRange_AsksAgain) {
    std::istringstream input("110\n50\n");
    CinRedirect redirect(input);

    testing::internal::CaptureStderr();//start recording error msg

    auto value = askForWallDensity();

    std::string errorOutput = testing::internal::GetCapturedStderr(); //stop recording

    ASSERT_TRUE(value.has_value());
    EXPECT_DOUBLE_EQ(value.value(), 50.0);

    EXPECT_NE(errorOutput.find("Invalid input: Value outside valid range."),std::string::npos);
}


// --- MudCount --- 
TEST(ConsoleTest, askForMudCount_ReturnsCorrectInput) {
    std::istringstream input("4\n");
    
    CinRedirect redirect(input);
    
    testing::internal::CaptureStdout();
    std::optional<double> value = askForMudCount();
    std::string output = testing::internal::GetCapturedStdout();

    ASSERT_TRUE(value.has_value());
    EXPECT_DOUBLE_EQ(value.value(), 4.0);
}

TEST(ConsoleTest, askForMudCount_EmptyInput_ReturnsDefaultMudCount) {
    std::istringstream input("\n");
    CinRedirect redirect(input);

    auto value = askForMudCount();

    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value.value(), GenerationSettings{}.mudCount);
}

TEST(ConsoleTest, askForMudCount_InvalidInputThenValidInput) {
    std::istringstream input("abc\n-5\n10\n");
    CinRedirect redirect(input);

    auto value = askForMudCount();

    ASSERT_TRUE(value.has_value());
    EXPECT_DOUBLE_EQ(value.value(), 10.0);
}

TEST(ConsoleTest, askForMudCount_EOF_ReturnsNullopt) {
    std::istringstream input("");
    CinRedirect redirect(input);

    auto value = askForMudCount();

    EXPECT_FALSE(value.has_value());
}


TEST(ConsoleTest, askForMudCount_Zero_ReturnsZero) {
    std::istringstream input("0\n");
    CinRedirect redirect(input);

    auto value = askForMudCount();

    ASSERT_TRUE(value.has_value());
    EXPECT_DOUBLE_EQ(value.value(), minBlobDensityRange);
}

TEST(ConsoleTest, askForMudCount_MaxValue_Returns100) {
    std::istringstream input("100\n");
    CinRedirect redirect(input);

    auto value = askForMudCount();

    ASSERT_TRUE(value.has_value());
    EXPECT_DOUBLE_EQ(value.value(), maxBlobDensityRange);
}

TEST(ConsoleTest, askForMudCount_InvalidRange_AsksAgain) {
    std::istringstream input("110\n50\n");
    CinRedirect redirect(input);

    testing::internal::CaptureStderr();//start recording error msg

    auto value = askForMudCount();

    std::string errorOutput = testing::internal::GetCapturedStderr(); //stop recording

    ASSERT_TRUE(value.has_value());
    EXPECT_DOUBLE_EQ(value.value(), 50.0);

    EXPECT_NE(errorOutput.find("Invalid input: Value outside valid range."),std::string::npos);
}
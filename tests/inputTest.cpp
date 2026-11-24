#include "console.hpp"
#include "input.hpp"

#include <gtest/gtest.h>

#include <iostream>
#include <string>
#include <optional>

//---- parseDouble ---
TEST(InputTest, parseDouble_BelowMin_ReturnsNullopt) {
    testing::internal::CaptureStderr();
    auto value = parseDouble("-100", minBlobDensityRange, maxBlobDensityRange);
    std::string errorOutput = testing::internal::GetCapturedStderr();

    EXPECT_FALSE(value.has_value());

    EXPECT_NE(errorOutput.find("Invalid input: "), std::string::npos);
    
    EXPECT_NE(errorOutput.find("Expected a value between 0 and 100."),  std::string::npos);
}

TEST(InputTest, parseDouble_AboveMax_ReturnsNullopt) {
    testing::internal::CaptureStderr();
    auto value = parseDouble("150", 0, 100.0);
    std::string errorOutput = testing::internal::GetCapturedStderr();

    EXPECT_FALSE(value.has_value());

    EXPECT_NE(errorOutput.find("Invalid input: "), std::string::npos);
    
    EXPECT_NE(errorOutput.find("Expected a value between 0 and 100."),  std::string::npos);
}

TEST(InputTest, parseDouble_EmptyString_ReturnsNullOpt) {
    auto value = parseDouble("", minBlobDensityRange, maxBlobDensityRange);

    EXPECT_EQ(value, std::nullopt);
}

TEST(InputTest, parseDouble_EmptySpaceString_ReturnsNullOpt) {
    auto value = parseDouble(" ", minBlobDensityRange, maxBlobDensityRange);

    EXPECT_EQ(value, std::nullopt);
}

TEST(InputTest, parseDouble_InvalidString_ReturnsNullOpt) {
    auto value = parseDouble("abc", minBlobDensityRange, maxBlobDensityRange);

    EXPECT_EQ(value, std::nullopt);
}

TEST(InputTest, parseDouble_ValidInput_ReturnsValue) {
    auto value = parseDouble("50.5", 0, 100);

    ASSERT_TRUE(value.has_value());
    EXPECT_DOUBLE_EQ(value.value(), 50.5);
}

TEST(InputTest, parseDouble_AtMin_ReturnsValue) {
    auto value = parseDouble("0", 0, 100);

    ASSERT_TRUE(value.has_value());
    EXPECT_DOUBLE_EQ(value.value(), 0.0);
}

TEST(InputTest, parseDouble_AtMax_ReturnsValue) {
    auto value = parseDouble("100", 0, 100);

    ASSERT_TRUE(value.has_value());
    EXPECT_DOUBLE_EQ(value.value(), 100.0);
}

TEST(InputTest, parseDouble_NumberWithTrailingCharacters_ReturnsNullopt) {
    auto value = parseDouble("50abc", 0, 100);

    EXPECT_FALSE(value.has_value());
}

TEST(InputTest, parseDouble_OutOfRange_ReturnsNullopt) {
    testing::internal::CaptureStderr();
    auto value = parseDouble("1e9999", 0, 100);
    std::string errorOutput = testing::internal::GetCapturedStderr();

    EXPECT_FALSE(value.has_value());

    EXPECT_NE(errorOutput.find("Input value is out of range."), std::string::npos);
}
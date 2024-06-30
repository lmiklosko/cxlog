#include "cxlog/ILogger.hpp"
#include "mocks/MockLogger.hpp"

#include <gtest/gtest.h>

using namespace cxlog;

class ILoggerTest : public ::testing::Test
{
};

/**
 * @brief Tests the Log Method
 */
TEST_F(ILoggerTest, Log)
{
    MockLogger l;
    EXPECT_CALL(l, Log(LogLevel::Warning, "MyMessage 125 str"));

    l.Log(LogLevel::Warning, "MyMessage {} {}", 125, "str");
}

TEST_F(ILoggerTest, LogWarning)
{
    MockLogger l;
    EXPECT_CALL(l, Log(LogLevel::Warning, "MyMessage 125 str"));

    l.LogWarning("MyMessage {} {}", 125, "str");
}
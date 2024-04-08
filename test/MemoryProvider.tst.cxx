#include "gtest/gtest.h"

#include <ranges>

#include "cxlog/MemoryProvider.hpp"

class MemoryProviderTest : public ::testing::Test
{
protected:
};

/**
 * @brief Tests the GetLogger Method
 * @expected GetLogger should never return null
 */
TEST_F(MemoryProviderTest, GetLogger)
{
    /*Arrange*/
    MemoryProvider provider(10);

    /*Act*/
    auto logger = provider.GetLogger("TestLogger");

    /*Assert*/
    EXPECT_NE(logger, nullptr);
}

/**
 * @brief Tests the GetLogger Method
 * @expected Calling GetLogger with the same name should return the same logger instance
 */
TEST_F(MemoryProviderTest, GetLogger_SameName)
{
    /*Arrange*/
    MemoryProvider provider(10);

    /*Act*/
    auto l1 = provider.GetLogger("TestLogger");
    auto l2 = provider.GetLogger("TestLogger");
    auto l3 = provider.GetLogger("OtherLogger");

    /*Assert*/
    EXPECT_EQ(l1, l2);
    EXPECT_NE(l1, l3);
}

/**
 * @brief Tests the GetName Method
 * @expected Must return "MemoryLogProvider"
 */
TEST_F(MemoryProviderTest, GetName)
{
    /*Arrange*/
    MemoryProvider provider(10);

    /*Act*/
    auto name = provider.GetName();

    /*Assert*/
    EXPECT_EQ(name, "MemoryProvider");
}

/**
 * @brief Tests the LogLines Method
 * @expected LogLines returns only lines appended since last call
 */
TEST_F(MemoryProviderTest, LogLines_Empty)
{
    /*Arrange*/
    MemoryProvider provider(10);

    /*Act*/
    provider.GetLogger("TestLogger")->Log(LogLevel::Info, "Test message");

    auto logLines = provider.LogLines();
    logLines = provider.LogLines();

    /*Assert*/
    EXPECT_EQ(logLines.size(), 0);
}

/**
 * @brief Tests the LogLines Method
 * @expected Must return the log lines from the logger
 */
TEST_F(MemoryProviderTest, LogLines_One)
{
    static constexpr const char* message = "This is a test message from unit test";

    /*Arrange*/
    MemoryProvider provider(10);

    auto logger = provider.GetLogger("TestLogger");
    logger->Log(LogLevel::Info, message);

    /*Act*/
    auto logLines = provider.LogLines();

    /*Assert*/
    EXPECT_EQ(logLines.size(), 1);
    EXPECT_NE(logLines[0].find(message), std::string::npos);
}

/**
 * @brief Tests the LogLines Method
 * @expected Must return only last 10 log lines from the logger
 */
TEST_F(MemoryProviderTest, LogLines_Overflow)
{
    const std::string message = "This is a test message from unit test";
    const auto containsMessage = [](const std::string& message) {
        return [message](const std::string &line) { return line.find(message) != std::string::npos; };
    };

    /*Arrange*/
    MemoryProvider provider(10);

    auto logger = provider.GetLogger("TestLogger");
    for (int i = 0; i < 20; i++)
    {
        logger->Log(LogLevel::Info, message + std::to_string(i));
    }

    /*Act*/
    auto logLines = provider.LogLines();

    /*Assert*/
    EXPECT_EQ(logLines.size(), 10);
    EXPECT_EQ(std::find_if(logLines.begin(), logLines.end(), containsMessage(message + "0")), logLines.end());
    EXPECT_EQ(std::find_if(logLines.begin(), logLines.end(), containsMessage(message + "9")), logLines.end());
    EXPECT_NE(std::find_if(logLines.begin(), logLines.end(), containsMessage(message + "10")), logLines.end());
    EXPECT_NE(std::find_if(logLines.begin(), logLines.end(), containsMessage(message + "19")), logLines.end());
}

/**
* @brief Tests the minimal logging level cutoff
*/
TEST_F(MemoryProviderTest, MinLogLevel)
{
    /* Arrange */
    MemoryProvider provider(10, LogLevel::Warning);
    auto l = provider.GetLogger("MyLog");

    /* Act */
    l->Log(LogLevel::Trace, "MyMessage");
    l->Log(LogLevel::Warning, "MyMessage");

    /* Assert */
    EXPECT_EQ(provider.LogLines().size(), 1);
}
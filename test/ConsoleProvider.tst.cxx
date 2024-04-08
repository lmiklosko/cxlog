#include <source_location>
#include "gtest/gtest.h"
#include "cxlog/ConsoleProvider.hpp"

class ConsoleProviderTest : public ::testing::Test 
{
};

/**
 * @brief Tests the Log Method
 */
TEST_F(ConsoleProviderTest, GetLogger) {
    static constexpr const char* LOG_MESSAGE = "MyMessage";

    /*Arrange*/
    std::stringstream ss;
    ConsoleProvider p(ss);

    /* Act */
    auto l1 = p.GetLogger("MyLog");
    auto l2 = p.GetLogger("MyOtherLog");
    auto l3 = p.GetLogger("MyLog");


    l1->Log(LogLevel::Warning, LOG_MESSAGE);

    /* Assert */
    EXPECT_NE(l1, l2);
    EXPECT_EQ(l1, l3);

    EXPECT_NE(ss.str().find(LOG_MESSAGE), std::string::npos);
}

/**
 * @brief Tests the IsEnable Method
 *
 * All Calls must return true
 */
TEST_F(ConsoleProviderTest, IsEnabledTest) {
    /*Act*/
    ConsoleProvider p(std::cout, LogLevel::Info);
    auto l = p.GetLogger("MyLog");

    /*Assert*/
    EXPECT_FALSE(l->IsEnabled(LogLevel::Trace));
    EXPECT_FALSE(l->IsEnabled(LogLevel::Debug));
    EXPECT_TRUE(l->IsEnabled(LogLevel::Info));
    EXPECT_TRUE(l->IsEnabled(LogLevel::Warning));
    EXPECT_TRUE(l->IsEnabled(LogLevel::Error));
    EXPECT_TRUE(l->IsEnabled(LogLevel::Critical));
}

/**
 * @brief Tests the GetName Method
 *
 * Must return "ConsoleLoggerProvider"
 */
TEST_F(ConsoleProviderTest, GetName) {
    /*Act*/
    ConsoleProvider provider(std::cout);

    /*Assert*/
    EXPECT_EQ(provider.GetName(), "ConsoleProvider");
}
































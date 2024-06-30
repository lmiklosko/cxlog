#include "cxlog/GLog.hpp"
#include <gtest/gtest.h>

using namespace cxlog;

TEST(GLog, Log)
{
    std::stringstream ss;

    auto keeper = std::cout.rdbuf();
    std::cout.rdbuf(ss.rdbuf());

    gLogFactory->CreateLogger("test")->Log(LogLevel::Info, "Hello, World!");

    std::cout.rdbuf(keeper);

    EXPECT_EQ(ss.str(), "[Info] test: Hello, World!\n");
}
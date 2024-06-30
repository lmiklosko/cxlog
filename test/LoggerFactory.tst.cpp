/**
 * @file LoggerFactory.tst.cpp
 * @brief Tests for LoggerFactory
 * @date 2024-01-17
 */


#include "cxlog/LoggerFactory.hpp"
#include "cxlog/MemoryProvider.hpp"
#include "cxlog/ConsoleProvider.hpp"

#include "mocks/MockLogger.hpp"

#include <gtest/gtest.h>
#include <algorithm>

using namespace cxlog;

class LoggerFactoryTest : public ::testing::Test
{
protected:
    static constexpr const char* LOG_MESSAGE = "Hello World!";
};

/**
 * @brief Tests the default constructor
 * Expects to have a single default rule (Trace log level)
 */
TEST_F(LoggerFactoryTest, Construction_1)
{
    /* Arrange */
    auto p = std::make_shared<MemoryProvider>(1);

    /* Act */
    LoggerFactory factory;

    /* Assert */
    /* We can test this by adding memory provider, creating logger and checking if it contains our message */
    factory.AddProvider(p);
    factory.CreateLogger("MyLog")->Log(LogLevel::Trace, "MyMessage");
    EXPECT_FALSE(p->LogLines().empty());
}

/**
 * @brief Tests the constructor with a list of providers
 * @expects to have a single default rule (Trace log level)
 *
 * @note Explicitly specifying providers is tested couple of tests below
 */
TEST_F(LoggerFactoryTest, Construction_2)
{
    auto p = std::make_shared<MemoryProvider>(1);
    LoggerFactory factory({
        p
    });

    factory.CreateLogger("MyLog")->Log(LogLevel::Trace, "MyMessage");
    EXPECT_FALSE(p->LogLines().empty());
}

/**
 * @brief Tests the constructor with a list of providers and options
 * @expects to have a single default rule (Trace log level)
 *
 * @note Explicitly specifying options is tested couple of tests below
 */
TEST_F(LoggerFactoryTest, Construction_3)
{
    auto p = std::make_shared<MemoryProvider>(2);
    LoggerFactory factory({ p }, {
        .MinLevel = LogLevel::Debug
    });

    auto l = factory.CreateLogger("MyLog");
    l->Log(LogLevel::Trace, "MyMessage");
    l->Log(LogLevel::Debug, "MyMessage");

    EXPECT_EQ(p->LogLines().size(), 1);
}

/**
 * @brief Tests creating logger without any providers.
 * @expects to have a logger which is not enabled for any log level
 */
TEST_F(LoggerFactoryTest, CreateLogger_NoProviders)
{
    LoggerFactory factory;
    auto logger = factory.CreateLogger("test");

    EXPECT_FALSE(logger->IsEnabled(LogLevel::Debug));
}

/**
 * @brief Tests AddProvider() function. This function should add a provider to the list of providers
 * and create an ILogger instance with this provider for all existing loggers.
 */
TEST_F(LoggerFactoryTest, CreateLogger_AddProvider)
{
    /* Arrange */
    LoggerFactory factory;
    auto logger = factory.CreateLogger("test");

    bool was_enabled_before_adding_provider = logger->IsEnabled(LogLevel::Debug);

    /* Act */
    auto p = std::make_shared<MemoryProvider>(1);
    factory.AddProvider(p);

    /* Assert */
    EXPECT_FALSE(was_enabled_before_adding_provider);
    ASSERT_TRUE(logger->IsEnabled(LogLevel::Debug));

    logger->Log(LogLevel::Error, LOG_MESSAGE);

    auto lines = p->LogLines();
    ASSERT_FALSE(lines.empty());
    EXPECT_NE(lines[0].find(LOG_MESSAGE), std::string::npos);
}

/**
 * @brief Logs a message from a logger to all available providers.
 * @expects to call Log() function on all ILoggers instances associated with logger from
 * all providers
 */
TEST_F(LoggerFactoryTest, LogMessage)
{
    auto p = std::make_shared<MemoryProvider>(1);
    LoggerFactory factory({
        p
    });

    /* Act */
    auto l = factory.CreateLogger("test");
    l->Log(LogLevel::Debug, LOG_MESSAGE);

    /* Assert */
    auto lines = p->LogLines();
    ASSERT_FALSE(lines.empty());
    EXPECT_NE(lines[0].find(LOG_MESSAGE), std::string::npos);
}

/**
 * @brief Tests that factory forwards log messages to all registered providers
 */
TEST_F(LoggerFactoryTest, LogMessage_MultipleProviders)
{
    /* Arrange */
    auto p1 = std::make_shared<MemoryProvider>(1);
    auto p2 = std::make_shared<MemoryProvider>(1);
    LoggerFactory factory({
        p1,
        p2
    });

    /* Act */
    auto l = factory.CreateLogger("test");
    l->Log(LogLevel::Debug, LOG_MESSAGE);

    /* Assert */
    EXPECT_FALSE(p1->LogLines().empty());
    EXPECT_FALSE(p2->LogLines().empty());
}

/**
 * @brief Tests the selective log level filtering based on the category name
 * @expects to call Log() function on the first provider and not call it on the second provider
 */
TEST_F(LoggerFactoryTest, LogLevels_Rule_Category)
{
    static constexpr const char* CATEGORY_1 = "Category1";
    static constexpr const char* CATEGORY_2 = "Category2";

    auto p1 = std::make_shared<MemoryProvider>(10);
    LoggerFactory factory({
            p1
        }, {
            .MinLevel = LogLevel::Critical,
            .Rules = {
                {
                    .CategoryName = CATEGORY_1,
                    .MinLevel = LogLevel::Warning,
                },
                {
                    .CategoryName = CATEGORY_2,
                    .MinLevel = LogLevel::Debug,
                }
            }
        }
    );

    auto logger_1 = factory.CreateLogger(CATEGORY_1);
    auto logger_2 = factory.CreateLogger(CATEGORY_2);

    logger_1->Log(LogLevel::Debug, LOG_MESSAGE);
    ASSERT_TRUE(p1->LogLines().empty());

    logger_2->Log(LogLevel::Debug, LOG_MESSAGE);
    EXPECT_EQ(p1->LogLines().size(), 1);
}

/**
 * @brief Tests the selective log level filtering based on the provider name
 * @expects to call Log() function on the first provider and not call it on the second provider
 */
TEST_F(LoggerFactoryTest, LogLevels_Rule_Provider)
{
    /* Arrange */
    std::stringstream ss;
    auto p1 = std::make_shared<MemoryProvider>(1);
    auto p2 = std::make_shared<ConsoleProvider>(ss);

    LoggerFactory factory(std::vector<std::shared_ptr<ILoggerProvider>>{
        p1,
        p2
    }, {
        .MinLevel = LogLevel::Critical,
        .Rules = {
            {
                .ProviderName = std::string(p1->GetName()),
                .MinLevel = LogLevel::Debug,
            },
            {
                .ProviderName = std::string(p2->GetName()),
                .MinLevel = LogLevel::Warning,
            }
        }
    });

    auto logger = factory.CreateLogger("test");

    /* Act */
    logger->Log(LogLevel::Debug, LOG_MESSAGE);

    /* Assert */
    EXPECT_FALSE(p1->LogLines().empty());
    EXPECT_TRUE(ss.str().empty());
}

/**
 * @brief Catching exceptions from interfaces
 * @expects When one of provider interfaces throws an exception, it should be caught and not rethrown
 * allowing other ILoggers to do their job. After all loggers finish, the exception should be rethrown
 * as aggregate exception.
 */
TEST_F(LoggerFactoryTest, ExceptionHandling)
{
    class MockProvider : public ILoggerProvider
    {
    public:
        [[nodiscard]] std::string_view GetName() const override { return "MockProvider"; }
        std::shared_ptr<ILogger> GetLogger(const std::string &name) override
        {
            auto l = std::make_shared<MockLogger>();
            ON_CALL(*l, IsEnabled).WillByDefault(::testing::Return(true));
            EXPECT_CALL(*l, Log).WillRepeatedly(::testing::Throw(std::exception()));

            return l;
        }
    };

    LoggerFactory factory({
        std::make_shared<MockProvider>()
    });

    auto logger = factory.CreateLogger("test");
    EXPECT_NO_THROW(logger->Log(LogLevel::Debug, LOG_MESSAGE));
}

TEST_F(LoggerFactoryTest, Common)
{
    /* This will mute LogLevel::to_string() code coverage errors */
    EXPECT_EQ(to_string(LogLevel::Critical), "Critical");
    EXPECT_EQ(to_string(LogLevel::Error), "Error");
    EXPECT_EQ(to_string(LogLevel::Warning), "Warning");
    EXPECT_EQ(to_string(LogLevel::Info), "Info");
    EXPECT_EQ(to_string(LogLevel::Debug), "Debug");
    EXPECT_EQ(to_string(LogLevel::Trace), "Trace");
}
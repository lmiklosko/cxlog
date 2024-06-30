#include "gtest/gtest.h"
#include "cxlog/FileProvider.hpp"

#include <fstream>

using namespace cxlog;

class FileProviderTest : public ::testing::Test
{
protected:
    static constexpr const char* PATH = "/tmp/FileProviderTest/";
    static constexpr const char* MESSAGE = "Message";

    static std::string dumpFile(const std::filesystem::path& path) {
        std::ifstream ifs(path);
        return {
                std::istreambuf_iterator<char>(ifs),
                std::istreambuf_iterator<char>()
        };
    }

    static std::vector<std::filesystem::path> listFiles(const std::filesystem::path& path) {
        std::vector<std::filesystem::path> files;
        for (const auto& entry : std::filesystem::directory_iterator(PATH))
        {
            files.push_back(entry.path());
        }

        return files;
    }

    static void SetUpTestSuite() {
        if (std::filesystem::exists(PATH))
        {
            for (const auto& entry : std::filesystem::directory_iterator(PATH))
                std::filesystem::remove_all(entry.path());
        }
        else
        {
            std::filesystem::create_directory(std::filesystem::path(PATH));
        }
    }

    static void TearDownTestSuite() {
        std::filesystem::remove_all(PATH);
    }

    void TearDown() override {
        for (const auto& entry : std::filesystem::directory_iterator(PATH))
            std::filesystem::remove_all(entry.path());
    }
};

/**
 * Default options should log everything into single file and enable all log levels
 */
TEST_F(FileProviderTest, Construct)
{
    {
        FileProvider provider((std::filesystem::path(PATH)));

        auto l = provider.GetLogger("MyLog");
        l->Log(LogLevel::Trace, MESSAGE);
    }

    auto files = listFiles(PATH);

    ASSERT_EQ(files.size(), 1);
    EXPECT_NE(dumpFile(files[0]).find(MESSAGE), std::string::npos);
}

TEST_F(FileProviderTest, Construct_InvalidOptions)
{
    FileProviderOptions opt = {
            .splitType = FileSplitType::NumMessages,
            .messagesCount = -125
    };

    EXPECT_THROW(FileProvider((std::filesystem::path(PATH)), opt), std::invalid_argument);
}

TEST_F(FileProviderTest, MinLogLevel)
{
    {
        FileProvider provider {
            std::filesystem::path(PATH),
            {
                .minLevel = LogLevel::Warning
            }
        };

        auto l = provider.GetLogger("MyLog");
        l->Log(LogLevel::Info, MESSAGE);
    }

    auto files = listFiles(PATH);

    ASSERT_EQ(files.size(), 1);
    EXPECT_EQ(dumpFile(files[0]).find(MESSAGE), std::string::npos);
}

TEST_F(FileProviderTest, MaxLines)
{
    static constexpr int numMessages = 2;
    {
        FileProvider provider {
            std::filesystem::path(PATH),
            {
                .splitType = FileSplitType::NumMessages,
                .messagesCount = numMessages
            }
        };

        auto l = provider.GetLogger("MyLog");
        for (int i = 0; i < numMessages + 1; ++i)
        {
            l->Log(LogLevel::Trace, MESSAGE);
        }
    }

    auto files = listFiles(PATH);
    EXPECT_EQ(files.size(), 2);
}
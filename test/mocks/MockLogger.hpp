#pragma once
#include "gmock/gmock.h"

#include "cxlog/ILogger.hpp"

class MockLogger : public cxlog::ILogger
{
public:
    MOCK_METHOD(void, Log, (cxlog::LogLevel level, const std::string& message), (override));
    MOCK_METHOD(bool, IsEnabled, (cxlog::LogLevel level), (const, noexcept, override));

    using cxlog::ILogger::Log;
};
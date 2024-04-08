#pragma once
#include "gmock/gmock.h"

#include "cxlog/ILogger.hpp"

class MockLogger : public ILogger
{
public:
    MOCK_METHOD(void, Log, (LogLevel level, const std::string& message, (const std::map<std::string, std::string>&)), (override));
    MOCK_METHOD(bool, IsEnabled, (LogLevel level), (const, noexcept, override));
};
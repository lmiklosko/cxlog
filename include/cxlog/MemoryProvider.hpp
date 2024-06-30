#pragma once
#include "cxlog/defs.hpp"
#include "cxlog/ILoggerProvider.hpp"

#include <deque>
#include <vector>
#include <map>

CXLOG_NAMESPACE_BEGIN

class CXLOG_API MemoryProvider : public ILoggerProvider
{
public:
    /**
     * @brief Constructor
     * @param numLines Number of lines of logs to keep in memory
     */
    explicit MemoryProvider(int numLines, LogLevel minLevel = LogLevel::Trace);

    /**
     * @brief Forward saved log lines from the logger. Clears the log lines inside the provider
     * @return Saved log lines
     */
    [[nodiscard]]
    std::vector<std::string> LogLines();

    /* ================ ILoggerProvider ================ */

    std::shared_ptr<ILogger> GetLogger(const std::string& name) override;

    [[nodiscard]]
    std::string_view GetName() const override;

private:
    struct SharedInfo;
    friend class MemoryLogger;

    std::shared_ptr<SharedInfo> _sharedInfo;
    std::map<std::string, std::shared_ptr<ILogger>> _loggers;
};

CXLOG_NAMESPACE_END
#pragma once
#include "cxlog/defs.hpp"
#include "cxlog/ILoggerProvider.hpp"

CXLOG_NAMESPACE_BEGIN

class CXLOG_API SyslogProvider : public ILoggerProvider
{
public:
    /**
     * Constructs new syslog provider
     *
     * @param minLevel
     */
    explicit SyslogProvider(LogLevel minLevel = LogLevel::Trace);

    /**
     * Constructs a logger instance with given name
     *
     * @param name Category name
     * @return Logger instance
     */
    std::shared_ptr<ILogger> GetLogger(const std::string& name) override;

    /**
     * Returns the name of the provider
     *
     * @return "SyslogProvider"
     */
    [[nodiscard]] std::string_view GetName() const override;

private:
    LogLevel _minLevel;
    std::map<std::string, std::shared_ptr<ILogger>> _loggers;
};

CXLOG_NAMESPACE_END
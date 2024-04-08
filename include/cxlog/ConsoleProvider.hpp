#pragma once
#include "cxlog/ILogger.hpp"
#include "cxlog/ILoggerProvider.hpp"

#include <string>
#include <map>
#include <memory>

/**
 * Console logger provider
 *
 * Represents the provider whose loggers will print all messages into the ostream provided in the constructor,
 * while being thread safe.
 */
class ConsoleProvider : public ILoggerProvider
{
public:
    /**
     * Constructs new console provider
     *
     * @param target ostream to write log messages to
     * @param minLevel minimum accepted log level messages
     */
    explicit ConsoleProvider(std::ostream& target, LogLevel minLevel = LogLevel::Trace);

    /**
     * Creates logger with given category name.
     *
     * @param name Category name, used to prefix all messages logged by this impl.
     * @note Multiple calls with same category name returns the same instance.
     */
    std::shared_ptr<ILogger> GetLogger(const std::string& name) override;

    /**
     * @return Provider name
     */
    [[nodiscard]]
    std::string_view GetName() const override;

private:
    std::map<std::string, std::shared_ptr<ILogger>> _loggers;

    std::ostream& _target;
    LogLevel _minLevel;
};
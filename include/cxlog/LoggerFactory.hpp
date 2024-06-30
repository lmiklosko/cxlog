#pragma once
#include "cxlog/defs.hpp"
#include "cxlog/ILoggerFactory.hpp"
#include "cxlog/ILogger.hpp"

#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <functional>
#include <optional>

CXLOG_NAMESPACE_BEGIN

/**
 * @brief Define rules for logging
 *
 * @details Rules are used to filter out loggers based on their name and category name. All log messages matching
 * the provider/category can be potentionally filtered out or set a minimum log level. This allows for fine control
 * over what is logged and what is not for every component of the application.
 */
struct LoggerRule
{
    std::optional<std::string> ProviderName;
    std::optional<std::string> CategoryName;
    std::optional<LogLevel> MinLevel;
    std::function<bool(std::string_view provider, std::string_view category, LogLevel level)> Filter;
};

/**
 * Logger factory options
 */
struct LoggerOptions
{
    LogLevel MinLevel { LogLevel::Trace };
    std::vector<LoggerRule> Rules;
};

class Logger;
class CXLOG_API LoggerFactory : public ILoggerFactory
{
public:
    LoggerFactory();
    explicit LoggerFactory(const std::vector<std::shared_ptr<ILoggerProvider>>& providers, LoggerOptions options = {});

    /**
     * @brief Create a logger with given category name
     * @param category Category name
     * @return
     *
     * @details When creating a logger, it creates Logger instance encapsulating all ILoggers from all* providers
     * associated with this factory. This instance forwards all logging messages to any enabled ILoggers.
     *
     * *Providers which are associated with this ILogger instance are filtered based on the LoggerRules.
     * If provider matches the rule (provider.GetName() == rule.ProviderName && CategoryName.contains(rule.CategoryName)
     * checks the output of Filter() function. If true, the provider is added to the list of providers for this logger.
     * otherwise, this provider is not used for this logger.
     */
    std::shared_ptr<ILogger> CreateLogger(const std::string& category) override;

    /**
     * @brief Registers a logger provider with this factory
     * @param provider Provider to add
     * @return self
     *
     * @details This function adds a logger provider to this factory. It also adds this provider to all existing
     * loggers created by this factory, while applying LoggerRules (For more information about logger rules see
     * CreateLogger).
     */
    ILoggerFactory& AddProvider(std::shared_ptr<ILoggerProvider> provider) override;

protected:
    [[nodiscard]]
    const LoggerRule* ApplyFilters(std::string_view Provider, std::string_view Category) const noexcept;

private:
    std::vector<std::shared_ptr<ILoggerProvider>> _providers;
    std::map<std::string, std::shared_ptr<Logger>> _loggers;
    LoggerOptions _options;
};

CXLOG_NAMESPACE_END
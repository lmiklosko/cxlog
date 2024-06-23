#include "cxlog/LoggerFactory.hpp"

#include "cxlog/ILogger.hpp"
#include "cxlog/ILoggerProvider.hpp"

#include <algorithm>
#include <utility>
#include <cassert>

struct LoggerInfo
{
    std::shared_ptr<ILoggerProvider> Provider;
    std::shared_ptr<ILogger> Logger;
    const LoggerRule* Rule;

    LoggerInfo(std::shared_ptr<ILoggerProvider> Provider, std::shared_ptr<ILogger> logger, const LoggerRule* rule = nullptr)
        : Provider(std::move(Provider))
        , Logger(std::move(logger))
        , Rule(rule)
    {
    }

    [[nodiscard]]
    bool IsEnabled(LogLevel level, std::string_view CategoryName) const noexcept
    {
        if (Rule)
        {
            if (Rule->MinLevel && level < Rule->MinLevel.value())
                return false;

            if (Rule->Filter && !Rule->Filter(Provider->GetName(), CategoryName, level))
                return false;
        }
        return true;
    }
};

class Logger : public ILogger
{
    std::vector<LoggerInfo> _loggers;
    std::string _category;

public:
    Logger(std::vector<LoggerInfo> loggers, std::string CategoryName)
        : _loggers(std::move(loggers))
        , _category(std::move(CategoryName))
    {
    }

    void Log(LogLevel level, const std::string& message, const std::map<std::string, std::string>& properties) noexcept override
    {
        if (_loggers.empty())
            return;

        for (const auto& loggerInfo : _loggers)
        {
            if (!loggerInfo.IsEnabled(level, _category))
                continue;

            /* Iterate over loggers. If enabled for current level, log message */
            try
            {
                loggerInfo.Logger->Log(level, message, properties);
            }
            catch (...)
            {
            }
        }
    }

    [[nodiscard]]
    bool IsEnabled(LogLevel level) const noexcept override
    {
        for (const auto& log : _loggers)
            if (log.IsEnabled(level, _category))
            {
                return true;
            }

        return false;
    }

    void AddLogger(LoggerInfo logger)
    {
        _loggers.emplace_back(std::move(logger));
    }
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

LoggerFactory::LoggerFactory()
{
    _options.Rules.emplace_back().MinLevel = _options.MinLevel;
}

LoggerFactory::LoggerFactory(const std::vector<std::shared_ptr<ILoggerProvider>>& providers, LoggerOptions options)
    : _providers(providers)
    , _options(std::move(options))
{
    /* If no default rule provided, create one based on the min level */
    auto it = std::find_if(_options.Rules.begin(), _options.Rules.end(), [](auto& r){ return !r.ProviderName && !r.CategoryName; });
    if (it == _options.Rules.end())
    {
        _options.Rules.emplace_back().MinLevel = _options.MinLevel;
    }
}

const LoggerRule *LoggerFactory::ApplyFilters(std::string_view Provider, std::string_view Category) const noexcept
{
    auto RuleFilter = [Provider,Category](const LoggerRule& rule)
    {
        if (rule.ProviderName && rule.ProviderName.value() != Provider)
            return false;

        if (rule.CategoryName && Category.find(rule.CategoryName.value()) == std::string::npos)
            return false;

        return true;
    };

    auto filter = std::find_if(_options.Rules.begin(), _options.Rules.end(), RuleFilter);

    /* There should always be at least one default rule. See LoggerFactory constructor() */
    assert(filter != _options.Rules.end());
    return &*filter;
}

std::shared_ptr<ILogger> LoggerFactory::CreateLogger(const std::string& category)
{
    auto& l = _loggers[category];
    if (!l)
    {
        l = std::make_shared<Logger>([&]()
        {
            std::vector<LoggerInfo> loggers;
            for (const auto& provider : _providers)
            {
                auto filters = ApplyFilters(provider->GetName(), category);
                loggers.emplace_back(provider, provider->GetLogger(category), filters);
            }

            return loggers;
        }(), category);
    }

    return l;
}

ILoggerFactory& LoggerFactory::AddProvider(std::shared_ptr<ILoggerProvider> provider)
{
    _providers.push_back(provider);
    for (auto& [category,logger] : _loggers)
    {
        logger->AddLogger({provider, provider->GetLogger(category), ApplyFilters(provider->GetName(), category)});
    }

    return *this;
}


#include <iostream>
#include "cxlog/ConsoleProvider.hpp"
std::unique_ptr<ILoggerFactory> gLogFactory = std::make_unique<LoggerFactory>(
    std::vector<std::shared_ptr<ILoggerProvider>> {
        std::make_shared<ConsoleProvider>(std::cout)
    }
);
#pragma once
#include "cxlog/defs.hpp"
#include "cxlog/ILogger.hpp"
#include "cxlog/ILoggerProvider.hpp"

#include <memory>

CXLOG_NAMESPACE_BEGIN

/**
 * @brief Interface for logger factory
 * @details ILoggerFactory is responsible for creating ILogger instances and managing ILoggerProviders
 * which are used by these loggers.
 */
class ILoggerFactory
{
public:
    virtual ~ILoggerFactory() = default;

    /**
     * @brief Create a logger with given category name
     * @param category Category name
     * @return Instance of ILogger
     *
     * @details While effects of this function are implementation specific, it is expected that this function
     * creates a logger instance which forwards all logging messages to all ILoggers from all ILoggerProviders
     * associated with this factory. Calling this function multiple times with the same category name should
     * return the same instance of ILogger.
     */
    virtual std::shared_ptr<ILogger> CreateLogger(const std::string& categoryName) = 0;

    /**
     * @brief Registers a logger provider with this factory
     * @param provider Provider to add
     * @return self
     *
     * @details This function adds a logger provider to this factory. It also adds this provider to all existing
     * loggers created by this factory. However, the real effect of this function is implementation specific.
     */
    virtual ILoggerFactory& AddProvider(std::shared_ptr<ILoggerProvider> provider) = 0;
};

CXLOG_NAMESPACE_END
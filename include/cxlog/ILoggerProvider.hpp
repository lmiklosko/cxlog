#pragma once
#include "cxlog/defs.hpp"
#include "cxlog/ILogger.hpp"

#include <memory>
#include <string>
#include <string_view>

CXLOG_NAMESPACE_BEGIN

/**
 * Interface for logger provider
 *
 * @details While /ref Logger factory controls how loggers are created, ILoggerProvider controls how the data
 * is handled. For example, it can write the data to a file, send it over the network, etc. Factories derive
 * their ILogger instances from ILoggerProvider.
 */
class ILoggerProvider
{
public:
    virtual ~ILoggerProvider() = default;

    /**
     * @brief Returns name of this provider
     * @return
     */
    [[nodiscard]]
    virtual std::string_view GetName() const = 0;

    /**
     * @brief Returns a logger for given category name associated with this provider
     * @param name Category name for the logger
     * @return
     */
    virtual std::shared_ptr<ILogger> GetLogger(const std::string& name) = 0;
};

CXLOG_NAMESPACE_END
#pragma once
#include "cxlog/defs.hpp"

#include <sstream>
#include <string>
#include <map>
#include <chrono>

CXLOG_NAMESPACE_BEGIN

/**
 * @brief Defines the severity of the log message
 *e
 * @details Log levels are ordered from low to critical (Trace < Debug < Info < Warning < Error < Critical).
 * From logger standpoint, there is no differentiating between levels. It is up to the ILogger instances to decide
 * what to do with the message based on the level.
 */
enum class LogLevel
{
    Trace,    /**< Trace messages help understand how the application code is executed */
    Debug,    /**< The debug log captures relevant detail of events that may be useful during software debugging */
    Info,     /**< This log level captures an event that occurred, but it does not have to affect operations */
    Warning,  /**< Indicates that an unexpected event has occurred in an application that may disrupt the process */
    Error,    /**< At least one system component is inoperable and other functionality may be affected */
    Critical  /**< A system component is inoperable which is causing a fatal error within the system. */
};

static constexpr const char* to_string(LogLevel level) noexcept
{
    switch (level)
    {
        case LogLevel::Trace: return "Trace";
        case LogLevel::Debug: return "Debug";
        case LogLevel::Info: return "Info";
        case LogLevel::Warning: return "Warning";
        case LogLevel::Error: return "Error";
        case LogLevel::Critical: return "Critical";
    }
    return "";
}

namespace details
{
    template<typename... Args>
    class IfConstexpr : public Args...
    {
    public:
        explicit IfConstexpr(Args&&... args)
            : Args(std::forward<Args>(args))...
        {
        }
        using Args::operator()...;
    };
}

/**
 * @brief Interface for logging
 *
 * @details ILogger is responsible for logging messages. It is expected that ILogger instances are created by
 * ILoggerFactory. This interface serves as a bridge between ILoggerFactory and ILoggerProvider, providing end
 * user with a single interface for logging.
 */
class CXLOG_API ILogger
{
public:
    virtual ~ILogger() = default;

    /**
     * @brief Log a message
     *
     * @param level Severity level (see /ref LogLevel)
     * @param message Message content
     */
    virtual void Log(LogLevel level, const std::string& message) = 0;

    /**
     * @brief Check if a given log level is enabled for this logger.
     *
     * @param level Level to be checked for
     * @return true if enabled, false otherwise
     */
    [[nodiscard]]
    virtual bool IsEnabled(LogLevel level) const noexcept = 0;

    /* ~~~~~~~~~~~~~~~~~~~~ Helpers - non overridable functions ~~~~~~~~~~~~~~~~~~~~ */

    template<typename... Args>
    void Log(LogLevel level, std::string format, Args&& ...args)
    {
        ([&, idx = 0ul](auto&& arg) mutable {
            idx = format.find("{}", idx);
            if (idx == std::string::npos)
                return;

            std::stringstream ss;
            details::IfConstexpr(
                [&ss](const std::chrono::system_clock::time_point& tp) { ss << tp.time_since_epoch().count(); },
                [&ss](const std::chrono::steady_clock::time_point& tp) { ss << tp.time_since_epoch().count(); },
                [&ss](const std::chrono::nanoseconds& ns) { ss << ns.count() << "ns"; },
                [&ss](const std::chrono::microseconds & ns) { ss << ns.count() << "us"; },
                [&ss](const std::chrono::milliseconds & ns) { ss << ns.count() << "ms"; },
                [&ss](const std::chrono::seconds & ns) { ss << ns.count() << "s"; },
                [&ss](const std::chrono::minutes & ns) { ss << ns.count() << "m"; },
                [&ss](const std::chrono::hours & ns) { ss << ns.count() << "h"; },
                [&ss](const auto& arg) { ss << arg; }
            )(arg);

            format.replace(idx, 2, ss.str());
        }(std::forward<Args>(args)), ...);

        Log(level, format);
    }

    template<typename... Args> inline void LogTrace(const std::string& message, Args&& ...args)
    { Log(LogLevel::Trace, message, std::forward<Args>(args)...); }

    template<typename... Args> inline void LogDebug(const std::string& message, Args&& ...args)
    { Log(LogLevel::Debug, message, std::forward<Args>(args)...); }

    template<typename... Args> inline void LogInfo(const std::string& message, Args&& ...args)
    { Log(LogLevel::Info, message, std::forward<Args>(args)...); }

    template<typename... Args> inline void LogWarning(const std::string& message, Args&& ...args)
    { Log(LogLevel::Warning, message, std::forward<Args>(args)...); }

    template<typename... Args> inline void LogError(const std::string& message, Args&& ...args)
    { Log(LogLevel::Error, message, std::forward<Args>(args)...); }

    template<typename... Args> inline void LogCritical(const std::string& message, Args&& ...args)
    { Log(LogLevel::Critical, message, std::forward<Args>(args)...); }
};

CXLOG_NAMESPACE_END
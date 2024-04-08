#pragma once

#include <string>
#include <source_location>
#include <experimental/source_location>
#include <map>

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

/**
 * @brief Interface for logging
 *
 * @details ILogger is responsible for logging messages. It is expected that ILogger instances are created by
 * ILoggerFactory. This interface serves as a bridge between ILoggerFactory and ILoggerProvider, providing end
 * user with a single interface for logging.
 */
class ILogger
{
public:
    virtual ~ILogger() = default;

    /**
     * @brief Log a message
     *
     * @param level Severity level (see /ref LogLevel)
     * @param message Message content
     * @param props extra data
     */
    virtual void Log(LogLevel level, const std::string& message, const std::map<std::string, std::string>& props = {}) = 0;  // NOLINT(google-default-arguments)

    /**
     * @brief Check if a given log level is enabled for this logger.
     *
     * @param level Level to be checked for
     * @return true if enabled, false otherwise
     */
    [[nodiscard]]
    virtual bool IsEnabled(LogLevel level) const noexcept = 0;
};
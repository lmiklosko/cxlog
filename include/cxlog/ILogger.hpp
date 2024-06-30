#pragma once
#include "cxlog/defs.hpp"

#include <sstream>
#include <string>
#include <map>

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
    template <class T, template <class...> class Template>
    struct is_specialization : std::false_type {};

    template <template <class...> class Template, class... Args>
    struct is_specialization<Template<Args...>, Template> : std::true_type {};

    struct Props
    {
        std::map<std::string, std::string> mapped;

        template<typename... Args, typename = std::enable_if_t<sizeof...(Args) != 0 && (is_specialization<Args, std::pair>{} || ...)>>
        Props(Args&& ...args) // NOLINT(*-explicit-constructor)
        {
            expo(std::forward<Args>(args)...);
        }

        template<typename T, typename...Args>
        void expo(T&& value, Args&&...others)
        {
            std::stringstream ss;
            ss << value.second;

            mapped[value.first] = ss.str();
            expo(std::forward<Args>(others)...);
        }

        inline void expo() noexcept
        {
        }
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
            ss << arg;

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
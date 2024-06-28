#pragma once
#include <sstream>
#include <string>
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
class ILogger
{
public:
    virtual ~ILogger() = default;

    /**
     * @brief Log a message
     *
     * @param level Severity level (see /ref LogLevel)
     * @param message Message content
     * @param props extra data as key/value pairs (stringified)
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

    /* ~~~~~~~~~~~~~~~~~~~~ Helpers - non overridable functions ~~~~~~~~~~~~~~~~~~~~ */

    /**
     * Accepts general key/value pairs and stringifies them before passing to them to log function
     * @param level Severity level (see /ref LogLevel)
     * @param message Log message
     * @param props extra data (generic key/value pairs)
     */
    void Log(LogLevel level, const std::string& message, details::Props&& props)
    {
        Log(level, message, props.mapped);
    }

    template<typename... Args>
    void Logc(LogLevel level, const std::string& message, Args&&...args)
    {
        std::stringstream ss;
        ((ss << args << " ") << ...);

        Log(level, message + ss.str());
    }

    template<typename... Args>
    void Logf(LogLevel level, const std::string& format, Args&& ...args)
    {
        char buf[format.size() * 2 + 1];
        snprintf(buf, sizeof(buf), format.c_str(), std::forward<Args>(args)...);

        Log(level, buf);
    }

    inline void Trace(const std::string& message, const std::map<std::string, std::string>& props = {})
    { Log(LogLevel::Trace, message, props); }

    inline void Debug(const std::string& message, const std::map<std::string, std::string>& props = {})
    { Log(LogLevel::Debug, message, props); }

    inline void Info(const std::string& message, const std::map<std::string, std::string>& props = {})
    { Log(LogLevel::Info, message, props); }

    inline void Warning(const std::string& message, const std::map<std::string, std::string>& props = {})
    { Log(LogLevel::Warning, message, props); }

    inline void Error(const std::string& message, const std::map<std::string, std::string>& props = {})
    { Log(LogLevel::Error, message, props); }

    inline void Critical(const std::string& message, const std::map<std::string, std::string>& props = {})
    { Log(LogLevel::Critical, message, props); }
};
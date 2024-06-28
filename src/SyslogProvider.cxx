#include "cxlog/SyslogProvider.hpp"

#include <syslog.h>

static constexpr int LogLevelToSyslogLevel(LogLevel level)
{
    switch (level)
    {
        case LogLevel::Trace:
        case LogLevel::Debug: return LOG_DEBUG;
        case LogLevel::Info: return LOG_INFO;
        case LogLevel::Warning: return LOG_WARNING;
        case LogLevel::Error: return LOG_ERR;
        case LogLevel::Critical: return LOG_CRIT;
    }

    return LOG_DEBUG;
}


class SyslogLogger : public ILogger
{
public:
    /* Forward message to syslog */
    void Log(LogLevel level, const std::string& message, const std::map<std::string, std::string>& props) override
    {
        std::stringstream ss;
        for (const auto& [key, value] : props)
        {
            ss << "\n" << key << ": " << value;
        }

        syslog(static_cast<int>(LogLevelToSyslogLevel(level)), "%s%s", message.c_str(), ss.str().c_str());
    }

    /* Log mask is controlled directly by syslog, always return true */
    [[nodiscard]] bool IsEnabled(LogLevel level) const noexcept override { return true; }
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

SyslogProvider::SyslogProvider(LogLevel minLevel)
    : _minLevel(minLevel)
{
    setlogmask(LOG_UPTO(static_cast<int>(LogLevelToSyslogLevel(minLevel))));
    openlog(nullptr, LOG_NDELAY, LOG_USER);
}

std::shared_ptr<ILogger> SyslogProvider::GetLogger(const std::string& name)
{
    auto& l = _loggers[name];
    if (!l)
    {
        l = std::make_shared<SyslogLogger>();
    }

    return l;
}

std::string_view SyslogProvider::GetName() const { return "SyslogProvider"; }
#include <string>
#include <iostream>
#include <utility>
#include <sstream>

#ifdef __ANDROID__
#include <android/log.h>
#endif

#include "cxlog/ConsoleProvider.hpp"

class ConsoleLogger : public ILogger
{
public:
    ConsoleLogger(std::string name, std::ostream& target, LogLevel minLevel)
        : _name(std::move(name))
        , _target(target)
        , _minLevel(minLevel)
    {
    }

    void Log(LogLevel level, const std::string& message, const std::map<std::string, std::string>& properties) override
    {
        std::ostringstream ss;

        ss << "[" << to_string(level) << "] " << _name << ": " << message << "\n";
        for(const auto & property : properties)
        {
            ss << "\t" << property.first << ": " << property.second << "\n";
        }

#ifdef __ANDROID__
        if (&_target == &std::cout)
        {
            auto str = ss.str();
            __android_log_write(LogLevelToAndroidLevel(level), _name.c_str(), str.c_str());
        }
#else
        _target << ss.str();
#endif /* __ANDROID__ */
    }

    [[nodiscard]]
    bool IsEnabled(LogLevel level) const noexcept override
    {
        return level >= _minLevel;
    }

private:
    const std::string _name;
    std::ostream& _target;
    LogLevel _minLevel;

#ifdef __ANDROID__
    static int LogLevelToAndroidLevel(LogLevel level)
    {
        switch (level) {
            case LogLevel::Trace: return ANDROID_LOG_VERBOSE;
            case LogLevel::Debug: return ANDROID_LOG_DEBUG;
            case LogLevel::Info: return ANDROID_LOG_INFO;
            case LogLevel::Warning: return ANDROID_LOG_WARN;
            case LogLevel::Error: return ANDROID_LOG_ERROR;
            case LogLevel::Critical: return ANDROID_LOG_FATAL;
        }
    }
#endif /* __ANDROID__ */
};

ConsoleProvider::ConsoleProvider(std::ostream &target, LogLevel minLevel)
    : _target(target)
    , _minLevel(minLevel)
{
}

std::shared_ptr<ILogger> ConsoleProvider::GetLogger(const std::string& name)
{
    auto& l = _loggers[name];
    if (!l)
    {
        l = std::make_shared<ConsoleLogger>(name, _target, _minLevel);
    }

    return l;
}

std::string_view ConsoleProvider::GetName() const
{
    return "ConsoleProvider";
}

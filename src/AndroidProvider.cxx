#include "cxlog/AndroidProvider.hpp"
#include <android/log.h>
#include <sstream>

struct AndroidProvider::SharedData
{
    LogLevel _minLevel;
};

class AndroidLogger : public ILogger
{
public:
    AndroidLogger(std::string name, std::shared_ptr<AndroidProvider::SharedData> info)
        : _name(std::move(name))
        , _info(std::move(info))
    {
    }

    void Log(LogLevel level, const std::string &message, const std::map<std::string, std::string>& props) override
    {
        std::ostringstream ss;

        ss << message << "\n";
        for(const auto & property : props)
        {
            ss << "\t" << property.first << ": " << property.second << "\n";
        }

        auto str = ss.str();
        __android_log_write(LogLevelToAndroidLevel(level), _name.c_str(), str.c_str());
    }

    [[nodiscard]] bool IsEnabled(LogLevel level) const noexcept override
    {
        return level >= _info->_minLevel;
    }

private:
    std::string _name;
    std::shared_ptr<AndroidProvider::SharedData> _info;

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
};

AndroidProvider::AndroidProvider(LogLevel minLevel)
    : _data(std::make_shared<SharedData>())
{
    _data->_minLevel = minLevel;
}

std::shared_ptr<ILogger> AndroidProvider::GetLogger(const std::string &name)
{
    auto& l = _loggers[name];
    if (!l)
    {
        l = std::make_shared<AndroidLogger>(name, _data);
    }

    return l;
}

std::string_view AndroidProvider::GetName() const
{
    return "AndroidProvider";
}
#include <string>
#include <iostream>
#include <utility>
#include <sstream>

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

        _target << ss.str();
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

#pragma once
#include "ILoggerProvider.hpp"
#include <map>

class AndroidProvider : public ILoggerProvider
{
public:
    explicit AndroidProvider(LogLevel minLevel);

    std::shared_ptr<ILogger> GetLogger(const std::string &name) override;

    [[nodiscard]]
    std::string_view GetName() const override;

private:
    friend class AndroidLogger;
    struct SharedData;

    std::shared_ptr<SharedData> _data;
    std::map<std::string, std::shared_ptr<ILogger>> _loggers;
};
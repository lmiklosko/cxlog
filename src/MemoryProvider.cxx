#include <sstream>
#include <utility>

#include "cxlog/MemoryProvider.hpp"


CXLOG_NAMESPACE_BEGIN


struct MemoryProvider::SharedInfo
{
    LogLevel minLevel;
    int numLines;

    std::deque<std::string> logLines;
};

class MemoryLogger : public ILogger
{
    std::shared_ptr<MemoryProvider::SharedInfo> _info;
    const std::string _name;
public:
    MemoryLogger(std::shared_ptr<MemoryProvider::SharedInfo> data, std::string  name)
        : _info(std::move(data)), _name(std::move(name))
    {
    }

    void Log(LogLevel level, const std::string& message) override
    {
        if (!IsEnabled(level))
            return;

        std::stringstream ss;
        ss << "[" << to_string(level) << "] " <<_name << ": " << message << "\n";

        /* Remove the first line if we have too many */
        if (_info->logLines.size() >= _info->numLines)
            _info->logLines.erase(_info->logLines.begin());

        _info->logLines.push_back(ss.str());
    }

    [[nodiscard]]
    bool IsEnabled(LogLevel level) const noexcept override
    {
        return level >= _info->minLevel;
    }
};


MemoryProvider::MemoryProvider(int numLines, LogLevel minLevel)
    : _sharedInfo(std::make_shared<SharedInfo>())
{
    _sharedInfo->minLevel = minLevel;
    _sharedInfo->numLines = numLines;
}

std::vector<std::string> MemoryProvider::LogLines()
{
    std::vector<std::string> lines;
    lines.reserve(_sharedInfo->logLines.size());

    std::copy(std::make_move_iterator(_sharedInfo->logLines.begin()),
              std::make_move_iterator(_sharedInfo->logLines.end()),
              std::back_inserter(lines));

    _sharedInfo->logLines.clear();
    return lines;
}

std::shared_ptr<ILogger> MemoryProvider::GetLogger(const std::string& name)
{
    auto& logger = _loggers[name];
    if (!logger)
    {
        logger = std::make_shared<MemoryLogger>(_sharedInfo, name);
    }

    return logger;
}

std::string_view MemoryProvider::GetName() const
{
    return "MemoryProvider";
}

CXLOG_NAMESPACE_END
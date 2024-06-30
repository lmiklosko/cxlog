#include <filesystem>
#include <iostream>
#include <utility>
#include <ctime>
#include <algorithm>
#include <fstream>
#include <sstream>

#include "cxlog/FileProvider.hpp"


CXLOG_NAMESPACE_BEGIN


static std::string MakeFileName(const std::filesystem::path& base)
{
    std::time_t time = std::time({});
    char timeString[std::size("yyyy-mm-ddThh:mm:ssZ")];
    std::strftime(std::data(timeString), std::size(timeString),
                  "%FT%TZ", std::gmtime(&time));

    std::replace_if(std::begin(timeString), std::end(timeString), [](char c){ return c == ':' || c == '.'; }, '-');

    int n = 0;
    std::string filename;

    do {
        filename = std::string(timeString) + (n == 0 ? "" : ("-" + std::to_string(n))) + ".log";
        ++n;
    } while (std::filesystem::exists(base / filename));

    return filename;
}


struct FileProvider::SharedData
{
    std::filesystem::path path;       /**< Basename to use for log files */
    FileProviderOptions opt;          /**< Provider options */

    std::ofstream file;               /**< File stream to write logs to */
    int messageCounter;               /**< Counter to use for log messages */
    int lastMessageDay;               /**< Day of month of last logged message */
};

class FileLogger : public ILogger
{
public:
    FileLogger(std::string name, std::shared_ptr<FileProvider::SharedData> data)
        : _name(std::move(name)), _sharedData(std::move(data))
    {
    }

    void Log(LogLevel level, const std::string& message) override
    {
        if (!IsEnabled(level))
            return;

        std::stringstream ss;
        ss << "[" << to_string(level) << "] " << _name << ": " << message << "\n";

        if (_sharedData->opt.splitType == FileSplitType::NumMessages)
        {
            if(++_sharedData->messageCounter > _sharedData->opt.messagesCount)
            {
                _sharedData->messageCounter = 0;
                _sharedData->file.flush();

                _sharedData->file = std::ofstream(_sharedData->path / MakeFileName(_sharedData->path));
            }
        }
        else if (_sharedData->opt.splitType == FileSplitType::Daily)
        {
            auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            auto tm = std::localtime(&tt);

            if (tm->tm_mday != _sharedData->lastMessageDay)
            {
                _sharedData->lastMessageDay = tm->tm_mday;
                _sharedData->file.open(_sharedData->path / MakeFileName(_sharedData->path));
            }
        }

        _sharedData->file << ss.str();
    }

    [[nodiscard]] bool IsEnabled(LogLevel level) const noexcept override
    {
        return level >= _sharedData->opt.minLevel;
    }

private:

    std::string _name;                                        /**< Logger name */
    std::shared_ptr<FileProvider::SharedData> _sharedData;    /**< Shared data for all loggers created by common provider */
};

FileProvider::FileProvider(std::filesystem::path where, FileProviderOptions opt)
    : _providerData(nullptr), _loggers()
{
    if (opt.splitType == FileSplitType::NumMessages && opt.messagesCount <= 0)
    {
        throw std::invalid_argument("FileProvider: messagesCount must be provided when splitType == NumMessages");
    }

    _providerData = std::make_shared<SharedData>();
    _providerData->path = where.replace_filename("");
    _providerData->opt = opt;
    _providerData->messageCounter = 0;

    _providerData->file.open(_providerData->path / MakeFileName(_providerData->path));
}

std::string_view FileProvider::GetName() const
{
    return "FileLogger";
}

std::shared_ptr<ILogger> FileProvider::GetLogger(const std::string& name)
{
    auto& l = _loggers[name];
    if (!l)
    {
        l = std::make_shared<FileLogger>(name, _providerData);
    }

    return l;
}

CXLOG_NAMESPACE_END
#pragma once
#include "cxlog/ILoggerFactory.hpp"
#include "cxlog/ILogger.hpp"

#include <filesystem>
#include <variant>

/**
 * Options for splitting log files
 */
enum class FileSplitType {
    None,           /**< Creates one big file with all log messages inside; (eg. Do not split) */
    NumMessages,    /**< Creates new file after specific number of messages has been reached (eg 1k messages per file).
                           If using this option, messageCount is required. (see @ref FileProviderOptions) */
    Daily,          /**< One log file for each day */
};

/**
 * File provider options.
 *
 * @brief Used to set minimum log level for the provider, file split type and more.
 */
struct FileProviderOptions
{
    LogLevel minLevel = LogLevel::Trace;            /**< Minimum level of messages to be accepted by this provider */
    FileSplitType splitType = FileSplitType::None;  /**< Options for file splitting */
    int messagesCount {-1};                         /**< Max number of messages to be logged per file. Doesn't have any
                                                          effect unless splitType == NumMessages. Must be positive. */
};

/**
 * File provider.
 *
 * @brief Logs all messages to a new file located on the path specified by the constructor.
 */
class FileProvider : public ILoggerProvider
{
public:
    explicit FileProvider(std::filesystem::path where, FileProviderOptions opt = {});

    /**
     * Creates logger with given category name.
     *
     * @param name Category name, used to prefix all messages logged by this impl.
     * @note Multiple calls with same category name returns the same instance.
     */
    std::shared_ptr<ILogger> GetLogger(const std::string& name) override;

    /**
     * @return Provider name
     */
    [[nodiscard]]
    std::string_view GetName() const override;
 
private:
    struct SharedData;
    friend class FileLogger;

    std::map<std::string, std::shared_ptr<ILogger>> _loggers;
    std::shared_ptr<SharedData> _providerData;  /**< Shared data for all loggers created by this provider */
};
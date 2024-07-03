# CXLOG
*Configurable logging interface for cpp projects*

CXLog is small, but extensive logging utility meant for c++ 
projects targeting multiple architectures. It supports Android, iOS and Linux.


## What and how?
Its architecture allows for easy configuration and extensibility. User can
define what and how gets logged. At its core, cxlog uses tags to identify where messages
are coming from. This can be any string uses chooses without any limitations. 

This message is then passed to a `Provider` object which is responsible for writing
the message to the desired output. The `Provider` object can be anything from a simple
`std::cout` to a file or a network socket. Multiple providers can be used at the same time.

To avoid unnecessary overhead when managing providers, `LoggerFactory` is used. Its instance
manages all loggers and providers. Multiple factories can be used at the same time.

Thus, it can be summed up as
* `ILoggerFactory` - Creates instances of ILogger, manages which providers are used
* `ILoggerProvider` - Writes messages to the desired output
* `ILogger` - Interface for logging messages

A default implementation of LoggerFactory interface is provided, which allows adding
and removing providers at runtime without affecting the logging process (i.e. no need to lock the logger when 
adding a new provider or re-create logger objects). It also directs all messages from logger to all providers registered
with its instance (although this can be changed by specifying logging rules, @see xxx).

Additionally, CXLog provides basic Provider implementations, such as ConsoleProvider, FileProvider
and MemoryProvider. MemoryProvider is useful for testing purposes, as it stores last X messages in memory.

In case of Android and iOS, CXLog ConsoleProvider uses platform-specific logging functions, such as 
`__android_log_print` and `NSLog` to write messages to the console.


## Usage
To use CXLog, you need to include the header file `cxlog.hpp` and link against the library `cxlog`.

### Basic usage
```cpp
#include <cxlog/LoggerFactory.hpp>

int main()
{
    // Create a logger factory
    auto factory = cxlog::LoggerFactory({
        std::make_shared<cxlog::ConsoleProvider>(std::cout)
    });

    // Create a logger
    auto logger = factory->CreateLogger("main");

    // Log a message
    logger->LogInfo("Hello, world!");

    return 0;
}
```
The snipept above creates a logger factory with a single provider, ConsoleProvider, which writes messages to the console.

CXLog supports restricting logging to a certain level. By default, all messages are logged. To configure this, you can
either use factory to set a global level during creation of a factory.
```cpp
auto factory = cxlog::LoggerFactory({
    std::make_shared<cxlog::ConsoleProvider>()
}, { cxlog::LogLevel::Info });
```

Additinally, you can set a level for each provider individually.
```cpp
auto factory = cxlog::LoggerFactory({
    std::make_shared<cxlog::ConsoleProvider>(std::cout, cxlog::LogLevel::Info)
});
```

### Advanced usage
LoggerFactory supports advanced logging rules to selectively override category log levels or to filter out messages.
This can be particularly useful when you want to log messages from a specific category to a specific provider only,
or allow specific provider to log all messages. 

Below snippet will log all messages to console and critical only to file

```cpp
cxlog::LoggerFactory factory({ 
       std::make_shared<cxlog::ConsoleProvider>(std::cout),
       std::make_shared<cxlog::FileProvider>("/tmp/example.log")
   }, {
        .MinLevel = LogLevel::Critical,
        .Rules = {
            {
                .ProviderName = "ConsoleLogger",
                .MinLevel = LogLevel::Trace,
            }
        }
    }
);

factory.CreateLogger("main")->LogInfo("This message will be logged to console only");
```

Rules allow to restrict messages for particular logger as well. This is particulary useful for debugging purposes, or
disabling less important messages in production builds.

```cpp
cxlog::LoggerFactory factory({ 
       std::make_shared<cxlog::ConsoleProvider>(std::cout)
   }, {
        .MinLevel = LogLevel::Critical,
        .Rules = {
            {
                .CategoryName = "main",
                .MinLevel = LogLevel::Trace,
            }
        }
    }
);

factory.CreateLogger("main")->LogInfo("This message will be logged to console");
factory.CreateLogger("other")->LogInfo("This message will not be logged");
```
#include "cxlog/GLog.hpp"
#include "cxlog/ConsoleProvider.hpp"
#include "cxlog/LoggerFactory.hpp"

#include <iostream>

static std::aligned_storage_t<sizeof(std::unique_ptr<cxlog::ILoggerFactory>),
                              alignof(std::unique_ptr<cxlog::ILoggerFactory>)> gLogFactoryStorage;

static int nUnits = 0;


namespace cxlog {
    GLogInitializer::GLogInitializer() {
        if (nUnits++ == 0) {
            new(&gLogFactoryStorage) std::unique_ptr<ILoggerFactory>(
                    std::make_unique<LoggerFactory>(
                            std::vector<std::shared_ptr<ILoggerProvider>> {
                                    std::make_shared<ConsoleProvider>(std::cout)
                            }
                    )
            );
        }
    }

    GLogInitializer::~GLogInitializer() {
        if (--nUnits == 0) {
            reinterpret_cast<std::unique_ptr<ILoggerFactory>&>(gLogFactoryStorage).~unique_ptr();
        }
    }


    CXLOG_API std::unique_ptr<ILoggerFactory>& gLogFactory =
            reinterpret_cast<std::unique_ptr<ILoggerFactory>&>(gLogFactoryStorage);
}
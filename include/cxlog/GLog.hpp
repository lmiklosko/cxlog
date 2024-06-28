#pragma once
#include "cxlog/ILoggerFactory.hpp"

#include <memory>

namespace cxlog {
    extern std::unique_ptr<ILoggerFactory>& gLogFactory;

    static class GLogInitializer {
    public:
        GLogInitializer();
        ~GLogInitializer();
    } gLogInitializer;
}
#pragma once
#include "cxlog/defs.hpp"
#include "cxlog/ILoggerFactory.hpp"

#include <memory>

CXLOG_NAMESPACE_BEGIN

extern CXLOG_API std::unique_ptr<ILoggerFactory>& gLogFactory;

static class CXLOG_API GLogInitializer {
public:
    GLogInitializer();
    ~GLogInitializer();
} gLogInitializer;

CXLOG_NAMESPACE_END
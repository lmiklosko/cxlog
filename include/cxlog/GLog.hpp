#pragma once
#include "cxlog/ILoggerFactory.hpp"

#include <memory>

extern std::unique_ptr<ILoggerFactory> gLogFactory;
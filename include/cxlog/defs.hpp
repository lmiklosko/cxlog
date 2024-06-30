#pragma once

#ifdef CXLOG_EXPORT_SYMBOLS
  #ifdef _WIN32
    #define CXLOG_API __declspec(dllexport)
    #define CXLOG_LOCAL __declspec(dllimport)
  #else
    #define CXLOG_API __attribute__((visibility("default")))
    #define CXLOG_LOCAL __attribute__((visibility("hidden")))
  #endif
#else
 #define CXLOG_API
 #define CXLOG_LOCAL
#endif

#define CXLOG_NAMESPACE_BEGIN namespace cxlog {
#define CXLOG_NAMESPACE_END }
cmake_minimum_required(VERSION 3.12)

project(cxlog
        VERSION 1.0.0
        DESCRIPTION "Highly configurable logging utility for c++"
        HOMEPAGE_URL ""
        LANGUAGES CXX)

option (ENABLE_PROVIDER_CONSOLE "Enable Console log provider support" ON)
option (ENABLE_PROVIDER_FILE "Enables File log provider support" ON)
option (ENABLE_PROVIDER_MEMORY "Enable Memory log provider support" ON)
option (ENABLE_PROVIDER_SYSLOG "Enable Syslog provider support" ON)
option (ENABLE_GLOG "Enable global logger factory" ON)
option (EXPORT_CXLOG_SYMBOLS "Export symbols for shared library" ON)
option (BUILD_TESTS "Build and run unit tests" OFF)

add_library(${PROJECT_NAME}
    src/LoggerFactory.cxx
    $<$<BOOL:${ENABLE_PROVIDER_CONSOLE}>:src/ConsoleProvider.cxx>
    $<$<BOOL:${ENABLE_PROVIDER_FILE}>:src/FileProvider.cxx>
    $<$<BOOL:${ENABLE_PROVIDER_MEMORY}>:src/MemoryProvider.cxx>
    $<$<BOOL:${ENABLE_PROVIDER_SYSLOG}>:src/SyslogProvider.cxx>
    $<$<BOOL:${ENABLE_GLOG}>:src/GLog.cxx>
)

target_include_directories(${PROJECT_NAME}
    PUBLIC include/
)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        $<$<BOOL:${EXPORT_CXLOG_SYMBOLS}>:CXLOG_EXPORT_SYMBOLS=1>
        CXLOG_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
        CXLOG_VERSION_MINOR=${PROJECT_VERSION_MINOR}
        CXLOG_VERSION_PATCH=${PROJECT_VERSION_PATCH}
)

if (ENABLE_PROVIDER_ANDROID)
    if (NOT ANDROID)
        message(FATAL_ERROR "Cannot enable android provider if not targeting android")
    endif ()

    target_link_libraries(${PROJECT_NAME}
            INTERFACE log
    )
endif ()

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
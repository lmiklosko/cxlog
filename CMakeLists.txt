cmake_minimum_required(VERSION 3.23)

project(cxlog
        VERSION 1.1.0
        DESCRIPTION "Highly configurable logging utility for c++"
        HOMEPAGE_URL "https://github.com/lmiklosko/cxlog"
        LANGUAGES CXX)

option (ENABLE_PROVIDER_CONSOLE "Enable Console log provider support" ON)
option (ENABLE_PROVIDER_FILE "Enables File log provider support" ON)
option (ENABLE_PROVIDER_MEMORY "Enable Memory log provider support" ON)
option (ENABLE_PROVIDER_SYSLOG "Enable Syslog provider support" ON)
option (ENABLE_GLOG "Enable global logger factory" ON)

option (CXLOG_FORCE_SHARED_LIB "Type of library to build" OFF)
option (CXLOG_EXPORT_SYMBOLS "Export symbols for shared library" ON)
option (BUILD_TESTS "Build and run unit tests" OFF)

# LIBRARY DEF
###############
if (CXLOG_FORCE_SHARED_LIB)
    add_library (${PROJECT_NAME} SHARED)
else ()
    # Do not explicitly specify STATIC, let the default decide
    add_library (${PROJECT_NAME})
endif ()

target_sources (${PROJECT_NAME}
    PRIVATE
        src/LoggerFactory.cxx
        $<$<BOOL:${ENABLE_PROVIDER_CONSOLE}>:src/ConsoleProvider.cxx>
        $<$<BOOL:${ENABLE_PROVIDER_FILE}>:src/FileProvider.cxx>
        $<$<BOOL:${ENABLE_PROVIDER_MEMORY}>:src/MemoryProvider.cxx>
        $<$<BOOL:${ENABLE_PROVIDER_SYSLOG}>:src/SyslogProvider.cxx>
        $<$<BOOL:${ENABLE_GLOG}>:src/GLog.cxx>

    PUBLIC
        FILE_SET HEADERS
        TYPE HEADERS
        BASE_DIRS ${PROJECT_SOURCE_DIR}/include
        FILES
            ${PROJECT_SOURCE_DIR}/include/cxlog/defs.hpp
            ${PROJECT_SOURCE_DIR}/include/cxlog/ILogger.hpp
            ${PROJECT_SOURCE_DIR}/include/cxlog/ILoggerProvider.hpp
            ${PROJECT_SOURCE_DIR}/include/cxlog/ILoggerFactory.hpp
            ${PROJECT_SOURCE_DIR}/include/cxlog/ConsoleProvider.hpp
            ${PROJECT_SOURCE_DIR}/include/cxlog/FileProvider.hpp
            ${PROJECT_SOURCE_DIR}/include/cxlog/MemoryProvider.hpp
            ${PROJECT_SOURCE_DIR}/include/cxlog/SyslogProvider.hpp
            ${PROJECT_SOURCE_DIR}/include/cxlog/LoggerFactory.hpp
            ${PROJECT_SOURCE_DIR}/include/cxlog/GLog.hpp
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
)

target_include_directories (${PROJECT_NAME}
    PUBLIC
        include/
)

target_compile_definitions (${PROJECT_NAME}
    PRIVATE
        $<$<BOOL:${CXLOG_EXPORT_SYMBOLS}>:CXLOG_EXPORT_SYMBOLS=1>
        CXLOG_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
        CXLOG_VERSION_MINOR=${PROJECT_VERSION_MINOR}
        CXLOG_VERSION_PATCH=${PROJECT_VERSION_PATCH}
)

target_compile_features (${PROJECT_NAME}
    PUBLIC
        cxx_std_17
)

target_compile_options (${PROJECT_NAME}
    PRIVATE
        -fPIC
        -ffunction-sections
        -fdata-sections
        -fvisibility=hidden
        $<$<COMPILE_LANGUAGE:CXX>:-fvisibility-inlines-hidden>
)

if (BUILD_SHARED_LIBS)
    target_link_options (${PROJECT_NAME}
        PRIVATE
            ${CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS}
    )
endif (BUILD_SHARED_LIBS)

target_link_options (${PROJECT_NAME}
    PRIVATE
        -Wl,--gc-sections
        -Wl,--strip-all
)

if (ANDROID)
    target_link_libraries(${PROJECT_NAME}
            INTERFACE log
    )
endif (ANDROID)

install(TARGETS ${PROJECT_NAME}
        FILE_SET HEADERS
        DESTINATION include/${PROJECT_NAME}-${PROJECT_VERSION}
        INCLUDES DESTINATION include/${PROJECT_NAME}-${PROJECT_VERSION}
)

# CONFIGURATION
#################
configure_file(${PROJECT_SOURCE_DIR}/cmake/template/config.cmake ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake @ONLY)
install(FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake DESTINATION lib/cmake/${PROJECT_NAME})

# TESTS
#########
if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif (BUILD_TESTS)
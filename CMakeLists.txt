cmake_minimum_required(VERSION 3.23)

project(cxlog
        VERSION 1.1.3
        DESCRIPTION "Multiplatform logging utility for C++"
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
    set(CXLOG_EXPORT_SYMBOLS ON)
else ()
    # Do not explicitly specify STATIC, let it be the default
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

get_target_property(target_type ${PROJECT_NAME} TYPE)
if (target_type STREQUAL "SHARED_LIBRARY")
    target_compile_options (${PROJECT_NAME}
            PRIVATE
            -fPIC
            -fvisibility=hidden
            $<$<COMPILE_LANGUAGE:CXX>:-fvisibility-inlines-hidden>
            $<$<NOT:$<PLATFORM_ID:Darwin>>:-ffunction-sections>
            $<$<NOT:$<PLATFORM_ID:Darwin>>:-fdata-sections>
            $<$<CONFIG:Release>:-O2>
    )

    # Apple's CLANG does not support --gc-sections and --strip-all
    if (NOT APPLE)
        target_link_options (${PROJECT_NAME}
            PRIVATE
                -Wl,--gc-sections
                -Wl,--strip-all
        )
    else ()
        target_link_options (${PROJECT_NAME}
            PRIVATE -Wl,-dead_strip
        )
        add_custom_command (TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND strip -S $<TARGET_FILE:${PROJECT_NAME}>
        )
    endif ()
endif ()

# Android needs to link against log library for __android_log_write
if (ANDROID)
    target_link_libraries(${PROJECT_NAME} PUBLIC log)
endif ()

install(TARGETS ${PROJECT_NAME}
        FILE_SET HEADERS
        DESTINATION include/${PROJECT_NAME}${PROJECT_VERSION_MAJOR}
        INCLUDES DESTINATION include/${PROJECT_NAME}${PROJECT_VERSION_MAJOR}
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
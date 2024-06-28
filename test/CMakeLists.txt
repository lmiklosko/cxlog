cmake_minimum_required(VERSION 3.12)

add_subdirectory(${PROJECT_SOURCE_DIR}/3rdparty/googletest ${CMAKE_BINARY_DIR}/googletest)

add_executable(tests
        LoggerFactory.tst.cpp
        ConsoleProvider.tst.cxx
        MemoryProvider.tst.cxx
        FileProvider.tst.cxx
        GLog.tst.cxx
)

target_link_libraries(tests gtest gtest_main gmock ${PROJECT_NAME})


add_test(NAME tests COMMAND tests)
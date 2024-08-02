include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
        ${PROJECT_NAME}_tests
        main_test.cpp
        argparser_unit_tests.cpp
        test_functions.cpp
        test_functions.hpp
        WeatherForecastIntegrationTestSuite.cpp
        WeatherForecastIntegrationTestSuite.hpp
        ArgParserUnitTestSuite.cpp
        ArgParserUnitTestSuite.hpp
        ForecastUnitTestSuite.cpp
        ForecastUnitTestSuite.hpp
        forecast_unit_tests.cpp
)

target_link_libraries(${PROJECT_NAME}_tests PUBLIC
        ui
        utils
        forecast
        nlohmann_json::nlohmann_json
        argparser
        GTest::gtest_main
        PRIVATE argparser_basic
)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug) # Tests should be built with Debug
endif()

message(STATUS "Tests build type: ${CMAKE_BUILD_TYPE}")

target_include_directories(${PROJECT_NAME}_tests PUBLIC ${PROJECT_SOURCE_DIR})

include(GoogleTest)

gtest_discover_tests(${PROJECT_NAME}_tests)

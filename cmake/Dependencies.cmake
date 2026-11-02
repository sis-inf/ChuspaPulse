include(FetchContent)

# cpp-httplib
FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.15.3
)

# nlohmann/json
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)

# toml++
FetchContent_Declare(
    tomlplusplus
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG v3.4.0
)

# SQLiteCpp
FetchContent_Declare(
    SQLiteCpp
    GIT_REPOSITORY https://github.com/SRombauts/SQLiteCpp.git
    GIT_TAG 3.3.1
)

FetchContent_MakeAvailable(
    httplib
    nlohmann_json
    tomlplusplus
    SQLiteCpp
)

if(CHUSPAPULSE_BUILD_TESTS)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )

    FetchContent_MakeAvailable(googletest)
endif()
add_executable(test_formatter_json
    tests/formatters/test_formatter_json.cpp
)

target_link_libraries(test_formatter_json
    gtest
    gtest_main
)

add_test(NAME test_formatter_json COMMAND test_formatter_json)
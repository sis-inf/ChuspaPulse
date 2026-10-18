enable_testing()
message(STATUS "Tests are enabled (PULSO_BUILD_TESTS=ON)")
include(GoogleTest)

# Apply AddressSanitizer flags if BUILD_WITH_ASAN is enabled
if(BUILD_WITH_ASAN)
    message(STATUS "Building tests with AddressSanitizer enabled")
    set(ASAN_FLAGS "-fsanitize=address,undefined -fno-omit-frame-pointer")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ASAN_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${ASAN_FLAGS}")
endif()

add_executable(test_types
    automatizados/unit/test_types.cpp
)
target_include_directories(test_types PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)
target_link_libraries(test_types PRIVATE
    GTest::gtest
    GTest::gtest_main
)
gtest_discover_tests(test_types)
 
add_executable(test_config
    test_config.cpp
)
target_include_directories(test_config PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)
target_link_libraries(test_config PRIVATE
    GTest::gtest
    GTest::gtest_main
)
gtest_discover_tests(test_config)

add_executable(test_storage
    storage/test_storage.cpp
)
target_include_directories(test_storage PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)
target_link_libraries(test_storage PRIVATE
    GTest::gtest
    GTest::gtest_main
    SQLiteCpp
    sqlite3
    pthread
    dl
)
gtest_discover_tests(test_storage)
add_executable(test_disk_usage
    automatizados/test_disk_usage.cpp
)
target_include_directories(test_disk_usage PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)
target_link_libraries(test_disk_usage PRIVATE
    GTest::gtest
    GTest::gtest_main
)
gtest_discover_tests(test_disk_usage)
add_executable(test_formatter_prometheus
    formatters/test_formatter_prometheus.cpp
    ${PROJECT_SOURCE_DIR}/src/formatters/formatter_prometheus.cpp
)
target_include_directories(test_formatter_prometheus PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)
target_link_libraries(test_formatter_prometheus PRIVATE
    GTest::gtest
    GTest::gtest_main
)
gtest_discover_tests(test_formatter_prometheus)

add_executable(test_ram_usage
    automatizados/test_ram_usage.cpp
    ${PROJECT_SOURCE_DIR}/src/collectors/memory/ram_usage.cpp
)
target_include_directories(test_ram_usage PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)
target_link_libraries(test_ram_usage PRIVATE
    GTest::gtest
    GTest::gtest_main
)
gtest_discover_tests(test_ram_usage)
add_executable(test_alerta_umbral
    automatizados/unit/test_alerta_umbral.cpp
    ${PROJECT_SOURCE_DIR}/src/alertas/alerta_umbral.cpp
)
target_include_directories(test_alerta_umbral PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)
target_link_libraries(test_alerta_umbral PRIVATE
    GTest::gtest
    GTest::gtest_main
)
gtest_discover_tests(test_alerta_umbral)

add_executable(test_gestor_alertas
    automatizados/unit/test_gestor_alertas.cpp
    ${PROJECT_SOURCE_DIR}/src/alertas/alerta_umbral.cpp
    ${PROJECT_SOURCE_DIR}/src/alertas/gestor_alertas.cpp
)
target_include_directories(test_gestor_alertas PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)
target_link_libraries(test_gestor_alertas PRIVATE
    GTest::gtest
    GTest::gtest_main
)
gtest_discover_tests(test_gestor_alertas)

add_executable(test_metrics
    automatizados/integration/test_metrics.cpp
    ${PROJECT_SOURCE_DIR}/src/http/handler_metrics.cpp
    ${PROJECT_SOURCE_DIR}/src/storage/storage.cpp
    ${PROJECT_SOURCE_DIR}/src/storage/schema.cpp
    ${PROJECT_SOURCE_DIR}/src/formatters/formatter_json.cpp
    ${PROJECT_SOURCE_DIR}/src/utils/logging/logger.cpp
)

target_include_directories(test_metrics PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(test_metrics PRIVATE
    GTest::gtest
    GTest::gtest_main
    httplib
    nlohmann_json
    SQLiteCpp
    sqlite3
)

if(UNIX)
    target_link_libraries(test_metrics PRIVATE pthread dl)
endif()
gtest_discover_tests(test_metrics)

add_executable(test_formatter_json
    formatters/test_formatter_json.cpp
    ${PROJECT_SOURCE_DIR}/src/formatters/formatter_json.cpp
)
target_include_directories(test_formatter_json PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)
target_link_libraries(test_formatter_json PRIVATE
    GTest::gtest
    GTest::gtest_main
    nlohmann_json
)
gtest_discover_tests(test_formatter_json)

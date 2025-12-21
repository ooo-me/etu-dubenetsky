cmake_minimum_required(VERSION 3.16)

# ============================================================================
# Настройка тестов
# ============================================================================

# Поиск GTest
find_package(GTest REQUIRED)

# Включаем тестирование
enable_testing()
include(GoogleTest)

# ============================================================================
# Тесты модели
# ============================================================================

add_executable(model_test
    model/model_test.cpp
)

target_link_libraries(model_test
    PRIVATE
        TariffSystemCore
        GTest::gtest
        GTest::gtest_main
)

target_include_directories(model_test PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

gtest_discover_tests(model_test)

# ============================================================================
# Тесты бизнес-логики
# ============================================================================

add_executable(core_test
    core/core_test.cpp
)

target_link_libraries(core_test
    PRIVATE
        TariffSystemCore
        GTest::gtest
        GTest::gtest_main
)

target_include_directories(core_test PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

gtest_discover_tests(core_test)

# ============================================================================
# Интеграционные тесты
# ============================================================================

add_executable(integration_test
    integration/integration_test.cpp
)

target_link_libraries(integration_test
    PRIVATE
        TariffSystemCore
        GTest::gtest
        GTest::gtest_main
)

target_include_directories(integration_test PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

gtest_discover_tests(integration_test)

# ============================================================================
# Цель для запуска всех тестов
# ============================================================================

add_custom_target(run_all_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --verbose
    DEPENDS model_test core_test integration_test
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Запуск всех тестов"
)

# ============================================================================
# Информация о тестах
# ============================================================================

message(STATUS "")
message(STATUS "===================================")
message(STATUS "Tests Configuration:")
message(STATUS "===================================")
message(STATUS "GTest Version: ${GTest_VERSION}")
message(STATUS "Test executables:")
message(STATUS "  - model_test (${CMAKE_CURRENT_SOURCE_DIR}/model/model_test.cpp)")
message(STATUS "  - core_test (${CMAKE_CURRENT_SOURCE_DIR}/core/core_test.cpp)")
message(STATUS "  - integration_test (${CMAKE_CURRENT_SOURCE_DIR}/integration/integration_test.cpp)")
message(STATUS "===================================")
message(STATUS "")

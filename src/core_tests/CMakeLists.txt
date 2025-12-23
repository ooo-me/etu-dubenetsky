# ============================================================================
# Core Library Tests
# Описание: Unit тесты для библиотеки core с использованием GTest
# ============================================================================

# Поиск GTest
find_package(GTest REQUIRED)

# Исходные файлы тестов
set(TEST_SOURCES
    TestFixture.h
    UnitTests.cpp
    ServiceTypeTests.cpp
    ExecutorTests.cpp
    TariffTests.cpp
    OrderTests.cpp
    OptimalExecutorTests.cpp
    CalculationTests.cpp
    IntegrationTests.cpp
)

# Создание исполняемого файла тестов
add_executable(core_tests ${TEST_SOURCES})

# Подключение директорий заголовочных файлов
target_include_directories(core_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Линковка библиотек
target_link_libraries(core_tests
    PRIVATE
        tariff_sys::core
        tariff_sys::db
        GTest::gtest
        GTest::gtest_main
)

# Включение CTest для интеграции с CMake
enable_testing()

# Регистрация тестов в CTest
add_test(NAME CoreTests COMMAND core_tests)

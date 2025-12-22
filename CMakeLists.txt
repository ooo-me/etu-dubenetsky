cmake_minimum_required(VERSION 3.16)

project(TariffSystem VERSION 1.0.0 LANGUAGES CXX)

# ============================================================================
# Настройки компилятора
# ============================================================================

# Требуется C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Настройки для MSVC
if(MSVC)
    add_compile_options(/W4 /WX- /permissive- /Zc:__cplusplus)
    add_compile_options(/utf-8)  # Поддержка UTF-8 для исходных файлов
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ============================================================================
# Поиск зависимостей
# ============================================================================

# Qt 5.15
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Sql
)

# PostgreSQL
find_package(PostgreSQL REQUIRED CONFIG)

# ============================================================================
# Исходные файлы
# ============================================================================

# Заголовочные файлы модели
set(MODEL_HEADERS
    src/model/Classifier.hpp
    src/model/Parameter.hpp
    src/model/Rule.hpp
    src/model/Service.hpp
    src/model/Tariff.hpp
    src/model/Order.hpp
)

# Заголовочные файлы бизнес-логики
set(CORE_HEADERS
    src/core/RuleEngine.hpp
    src/core/CostCalculator.hpp
    src/core/OptimalSearch.hpp
)

# Заголовочные файлы утилит
set(UTILS_HEADERS
    src/utils/Types.hpp
    src/utils/Database.hpp
    src/utils/Repository.hpp
)

# GUI заголовочные файлы (будут добавлены позже)
set(GUI_HEADERS
    # src/gui/MainWindow.hpp
    # src/gui/ServiceCatalog.hpp
    # src/gui/TariffEditor.hpp
    # src/gui/OrderForm.hpp
    # src/gui/SearchWidget.hpp
)

# GUI исходные файлы (будут добавлены позже)
set(GUI_SOURCES
    # src/gui/MainWindow.cpp
    # src/gui/ServiceCatalog.cpp
    # src/gui/TariffEditor.cpp
    # src/gui/OrderForm.cpp
    # src/gui/SearchWidget.cpp
)

# Главный файл приложения (будет добавлен позже)
# set(MAIN_SOURCE src/main.cpp)

# ============================================================================
# Библиотека модели и логики
# ============================================================================

add_library(TariffSystemCore INTERFACE)

target_include_directories(TariffSystemCore INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${PostgreSQL_INCLUDE_DIRS}
)

target_sources(TariffSystemCore INTERFACE
    ${MODEL_HEADERS}
    ${CORE_HEADERS}
    ${UTILS_HEADERS}
)

# ============================================================================
# Исполняемый файл приложения (временно закомментирован)
# ============================================================================

# add_executable(TariffSystem
#     ${MAIN_SOURCE}
#     ${GUI_SOURCES}
#     ${GUI_HEADERS}
# )

# target_link_libraries(TariffSystem
#     TariffSystemCore
#     Qt6::Core
#     Qt6::Gui
#     Qt6::Widgets
#     Qt6::Sql
#     ${PostgreSQL_LIBRARIES}
# )

# target_include_directories(TariffSystem PRIVATE
#     ${CMAKE_CURRENT_SOURCE_DIR}/src
#     ${PostgreSQL_INCLUDE_DIRS}
# )

# ============================================================================
# Установка
# ============================================================================

# install(TARGETS TariffSystem
#     RUNTIME DESTINATION bin
# )

# Установка SQL скриптов
install(DIRECTORY database/
    DESTINATION share/tariff-system/database
    FILES_MATCHING
    PATTERN "*.sql"
)

# Установка документации
install(DIRECTORY docs/
    DESTINATION share/tariff-system/docs
    FILES_MATCHING
    PATTERN "*.md"
    PATTERN "*.puml"
    PATTERN "*.mmd"
)

# ============================================================================
# Тесты (опционально)
# ============================================================================

option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# ============================================================================
# Информация о сборке
# ============================================================================

message(STATUS "")
message(STATUS "===================================")
message(STATUS "TariffSystem Configuration:")
message(STATUS "===================================")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Qt6 Version: ${Qt6_VERSION}")
message(STATUS "PostgreSQL Found: ${PostgreSQL_FOUND}")
if(PostgreSQL_FOUND)
    message(STATUS "PostgreSQL Include: ${PostgreSQL_INCLUDE_DIRS}")
    message(STATUS "PostgreSQL Library: ${PostgreSQL_LIBRARIES}")
endif()
message(STATUS "===================================")
message(STATUS "")

# ============================================================================
# Дополнительные настройки
# ============================================================================

# Генерация compile_commands.json для IDE
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Цветной вывод для Ninja
if(CMAKE_GENERATOR STREQUAL "Ninja")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_compile_options(-fdiagnostics-color=always)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        add_compile_options(-fcolor-diagnostics)
    endif()
endif()

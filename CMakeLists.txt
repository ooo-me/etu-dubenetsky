cmake_minimum_required(VERSION 3.16)

project(TariffSystem VERSION 1.0.0 LANGUAGES CXX)

# ============================================================================
# Настройки компилятора
# ============================================================================

# Требуется C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

add_subdirectory(src)

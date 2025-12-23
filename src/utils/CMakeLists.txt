# Утилиты и работа с БД
add_library(TariffUtils STATIC
    Utils.cpp
    Types.hpp
    Database.hpp
    Repository.hpp
)

target_include_directories(TariffUtils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${PostgreSQL_INCLUDE_DIRS}
)

target_link_libraries(TariffUtils PUBLIC
    TariffModel
    PostgreSQL::PostgreSQL
)

target_compile_features(TariffUtils PUBLIC cxx_std_20)

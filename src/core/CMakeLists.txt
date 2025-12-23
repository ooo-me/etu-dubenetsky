set(CORE_SOURCES
    include/core/TariffService.h
    include/core/Models.h
    src/TariffService.cpp
)

add_library(core STATIC ${CORE_SOURCES})

target_include_directories(core
    PUBLIC
        include
    PRIVATE
        include/core
        src
)

target_link_libraries(core
    PUBLIC
        tariff_sys::db
)

add_library(tariff_sys::core ALIAS core)

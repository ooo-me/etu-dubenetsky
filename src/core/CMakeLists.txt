# Бизнес-логика
add_library(TariffCore STATIC
    Core.cpp
    RuleEngine.hpp
    CostCalculator.hpp
    OptimalSearch.hpp
    DomainService.hpp
)

target_include_directories(TariffCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(TariffCore PUBLIC
    TariffModel
)

target_compile_features(TariffCore PUBLIC cxx_std_20)

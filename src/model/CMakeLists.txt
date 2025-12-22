# Модель предметной области
add_library(TariffModel STATIC
    Model.cpp
    Classifier.hpp
    Parameter.hpp
    Rule.hpp
    Service.hpp
    Tariff.hpp
    Order.hpp
)

target_include_directories(TariffModel PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_compile_features(TariffModel PUBLIC cxx_std_20)

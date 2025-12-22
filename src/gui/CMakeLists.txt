# GUI компоненты
add_library(TariffGUI STATIC
    MainWindow.cpp
    MainWindow.hpp
)

target_include_directories(TariffGUI PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(TariffGUI PUBLIC
    TariffSystemCore
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
)

target_compile_features(TariffGUI PUBLIC cxx_std_20)

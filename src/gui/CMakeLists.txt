set(GUI_SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/dialogs/ServiceTypeDialog.cpp
    src/dialogs/ServiceTypeDialog.h
    src/dialogs/ExecutorDialog.cpp
    src/dialogs/ExecutorDialog.h
    src/dialogs/TariffDialog.cpp
    src/dialogs/TariffDialog.h
    src/dialogs/OrderDialog.cpp
    src/dialogs/OrderDialog.h
    src/dialogs/ParameterDialog.cpp
    src/dialogs/ParameterDialog.h
    src/dialogs/UnitDialog.cpp
    src/dialogs/UnitDialog.h
    src/dialogs/CoefficientDialog.cpp
    src/dialogs/CoefficientDialog.h
    src/dialogs/OptimalSearchDialog.cpp
    src/dialogs/OptimalSearchDialog.h
)

add_executable(tariff_gui ${GUI_SOURCES})

target_include_directories(tariff_gui
    PRIVATE
        src
)

target_link_libraries(tariff_gui
    PRIVATE
        tariff_sys::core
        tariff_sys::db
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Sql
)

set_target_properties(tariff_gui PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)



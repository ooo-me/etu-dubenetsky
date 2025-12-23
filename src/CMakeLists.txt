add_subdirectory(db)
add_subdirectory(core)
add_subdirectory(gui)

# Опционально собирать тесты
if(BUILD_TESTING)
    add_subdirectory(core_tests)
endif()

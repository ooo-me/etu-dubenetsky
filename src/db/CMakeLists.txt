
set(SQL_FILES
    "${CMAKE_SOURCE_DIR}/database/procedures/calculator/calculator.sql"
    "${CMAKE_SOURCE_DIR}/database/procedures/constructor/constructor.sql"
    "${CMAKE_SOURCE_DIR}/database/procedures/utils/utils.sql"
    "${CMAKE_SOURCE_DIR}/database/schema/01_tables.sql"
    "${CMAKE_SOURCE_DIR}/database/schema/02_indexes.sql"
    "${CMAKE_SOURCE_DIR}/database/test-data/01_classifiers.sql"
    "${CMAKE_SOURCE_DIR}/database/test-data/02_tariffs.sql"
)

set(TASK_FILES
    "${CMAKE_SOURCE_DIR}/docs/data.txt"
    "${CMAKE_SOURCE_DIR}/docs/task.txt"
    "${CMAKE_SOURCE_DIR}/docs/addition1.txt"
    "${CMAKE_SOURCE_DIR}/docs/addition2.txt"
    "${CMAKE_SOURCE_DIR}/docs/example.txt"
)

source_group(TREE ${CMAKE_SOURCE_DIR}/database FILES ${SQL_FILES})
source_group(TREE ${CMAKE_SOURCE_DIR}/docs FILES ${TASK_FILES})

add_library(db STATIC
    ${SQL_FILES}
    ${TASK_FILES}
    include/db/Database.h
    include/db/DbApi.h
    src/Database.cpp
    src/DbApi.cpp
)

target_include_directories(db
	PUBLIC
		include
	PRIVATE
		include/db
		src
)

target_link_libraries(db PRIVATE
    PostgreSQL::PostgreSQL
)

add_library(tariff_sys::db ALIAS db)

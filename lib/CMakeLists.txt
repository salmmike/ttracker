set(INCDIR ${CMAKE_CURRENT_LIST_DIR}/include/)
set(LIBHEADERS
    ${INCDIR}ttracker.h
    ${INCDIR}ttracker_database.h
    ${INCDIR}ttracker_exception.h
    ${INCDIR}database_driver.h
    ${INCDIR}ttracker_timecalc.h
)


set(LIBSOURCES
    ${CMAKE_CURRENT_LIST_DIR}/ttracker.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ttracker_database.cpp
    ${CMAKE_CURRENT_LIST_DIR}/database_driver.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ttracker_timecalc.cpp
)

set(LIBNAME ${PROJECT_NAME}lib)


include_directories(${INCDIR})
add_library(${LIBNAME} STATIC ${LIBSOURCES})
target_include_directories(${LIBNAME} PUBLIC ${INCDIR})


find_package (SQLite3)

if (SQLITE3_FOUND)
  target_link_libraries(${LIBNAME} LINK_PUBLIC sqlite3)
else()
    message("sqlite3 not found!")
endif (SQLITE3_FOUND)

find_package(fmt)
target_link_libraries(${LIBNAME} LINK_PUBLIC fmt::fmt)

set(PATHNAME ${PROJECT_NAME})

target_compile_definitions(
    ${LIBNAME} PUBLIC
    -DTTRACKER_NAME_DEF=${PATHNAME}
    -DTTRACKER_VERSION=${PROJECT_VERSION}
)

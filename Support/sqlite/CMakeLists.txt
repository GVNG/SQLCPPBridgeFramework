PROJECT(sqlite3)
cmake_minimum_required(VERSION 3.15)

include_directories(${CMAKE_SOURCE_DIR}/src)
add_library(${PROJECT_NAME} STATIC sqlite3.c sqlite3.h sqlite3ext.h)

#set_target_properties(sqlite PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

add_definitions(-DSQLITE_ENABLE_RTREE)
add_definitions(-DSQLITE_ENABLE_FTS4)
add_definitions(-DSQLITE_ENABLE_FTS5)
add_definitions(-DSQLITE_ENABLE_JSON1)
add_definitions(-DSQLITE_ENABLE_RBU)
add_definitions(-DSQLITE_ENABLE_STAT4)

# Uncomment this for single-threaded variant (faster)
#add_definitions(-DSQLITE_THREADSAFE=0)


install(FILES sqlite3.h sqlite3ext.h DESTINATION include)
install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib)

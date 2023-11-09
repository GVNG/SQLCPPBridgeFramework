cmake_minimum_required ( VERSION 3.15...3.16.2 )

set( project_version 1.1.15 )
set( project_description "SQL to C++ bridge written in modern C++" )
set( project_homepage "https://github.com/GVNG/SQLCPPBridgeFramework" )

cmake_policy(SET CMP0135 NEW)

project ( SQLCppBridge
  VERSION      "${project_version}"
  DESCRIPTION  "${project_description}"
  HOMEPAGE_URL "${project_homepage}"
  LANGUAGES    C CXX )

set(SQLITE_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/sqlite)
set(SQLITE_SCRIPT_FILE ${CMAKE_CURRENT_LIST_DIR}/Support/sqlite/CMakeLists.txt)

# possible options
option ( BUILD_SHARED    "Build shared library instead of static" OFF )
option ( BUILD_FRAMEWORK "Build framework" OFF )
option ( REQUIRE_SQLITE  "Fetch sqlite" ON )

# if the user specified a platform to build for (i.e. OS)
# we assume the user wants a framework
get_property( PLATFORM_SPECIFIED 
  VARIABLE PROPERTY PLATFORM  
  SET )

if ( PLATFORM_SPECIFIED )  
  set ( BUILD_FRAMEWORK ON )
endif()

file ( GLOB_RECURSE ALL_SRC "SQLBridge/src/*.cpp" )
file ( GLOB_RECURSE HEADERS "SQLBridge/include/sqlcppbridge/*.h" )

if ( BUILD_SHARED )
  add_library ( ${PROJECT_NAME} SHARED "${ALL_SRC}" "${HEADERS}" )
  set ( REQUIRE_SQLITE ON ) # sqlite linkage is required for shared library
else()
  add_library ( ${PROJECT_NAME} STATIC "${ALL_SRC}" "${HEADERS}" )  
endif()


MESSAGE ( STATUS "${PROJECT_NAME} Options:" )
MESSAGE ( STATUS "----" )
MESSAGE ( STATUS "Build shared library:   " ${BUILD_SHARED} )
MESSAGE ( STATUS "Build framework:        " ${BUILD_FRAMEWORK} )
MESSAGE ( STATUS "Fetch SQLite3:          " ${REQUIRE_SQLITE} )
MESSAGE ( STATUS "----" )

if ( REQUIRE_SQLITE )
    include(FetchContent)
    FetchContent_Declare(
        sqlite3430100
        URL https://sqlite.org/2023/sqlite-amalgamation-3430100.zip
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${SQLITE_INSTALL_LOCATION} -DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}
        PATCH_COMMAND ${CMAKE_COMMAND} -E copy "${SQLITE_SCRIPT_FILE}" "."
    )
    FetchContent_MakeAvailable(sqlite3430100)
    include_directories(${SQLITE_INSTALL_LOCATION}/include)
    add_dependencies(${PROJECT_NAME} SQLite3)
else()
    find_package(SQLite3 REQUIRED)
endif()

include_directories(SQLBridge/include/sqlcppbridge)
add_library ( lib::SQLCppBridge ALIAS ${PROJECT_NAME} )

target_compile_features ( ${PROJECT_NAME}
    PRIVATE  cxx_std_14 )

target_include_directories ( ${PROJECT_NAME}
    PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/SQLBridge/include"
)

set_target_properties( ${PROJECT_NAME} PROPERTIES
  FRAMEWORK ${BUILD_FRAMEWORK}
  FRAMEWORK_VERSION C
  VERSION           ${project_version}
  CXX_STANDARD      14
  PUBLIC_HEADER     "${HEADERS}" )

if ( REQUIRE_SQLITE )

  if ( BUILD_SHARED )
    target_link_libraries (${PROJECT_NAME}
        SQLite3) # link right into the shared library
  else()
    target_link_libraries (${PROJECT_NAME}
        PUBLIC SQLite3) # just set as requirement for clients
  endif()
endif()

# installation
install ( TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include/sqlcppbridge)

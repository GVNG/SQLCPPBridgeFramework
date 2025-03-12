cmake_minimum_required ( VERSION 3.15...3.16.2 )

set( project_version 1.1.19 )
set( project_description "SQL to C++ bridge written in modern C++" )
set( project_homepage "https://github.com/GVNG/SQLCPPBridgeFramework" )

cmake_policy(SET CMP0135 NEW)

project ( SQLCppBridge
  VERSION      "${project_version}"
  DESCRIPTION  "${project_description}"
  HOMEPAGE_URL "${project_homepage}"
  LANGUAGES    C CXX )

# possible options
option ( BUILD_SHARED    "Build shared library instead of static" OFF )
option ( BUILD_FRAMEWORK "Build framework" OFF )
option ( FETCH_SQLITE    "Fetch sqlite" ON )

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

set (SQLITE3MC_BUILD_SHELL OFF)
if ( BUILD_SHARED )
  add_library ( ${PROJECT_NAME} SHARED "${ALL_SRC}" "${HEADERS}" )
  set ( FETCH_SQLITE ON ) # sqlite linkage is required for shared library
  set (SQLITE3MC_STATIC OFF)
else()
  add_library ( ${PROJECT_NAME} STATIC "${ALL_SRC}" "${HEADERS}" )
  set (SQLITE3MC_STATIC ON)
endif()


MESSAGE ( STATUS )
MESSAGE ( STATUS "${PROJECT_NAME} Options:" )
MESSAGE ( STATUS "---------------------------------" )
MESSAGE ( STATUS "Build shared library:         " ${BUILD_SHARED} )
MESSAGE ( STATUS "Build framework:              " ${BUILD_FRAMEWORK} )
MESSAGE ( STATUS "Fetch SQLite3MultipleCiphers: " ${FETCH_SQLITE} )
MESSAGE ( STATUS "---------------------------------" )

if ( FETCH_SQLITE )
    include(FetchContent)
    FetchContent_Declare(
        SQLite3MultipleCiphers
        GIT_REPOSITORY https://github.com/utelle/SQLite3MultipleCiphers.git
        GIT_TAG v2.1.0
    )
    FetchContent_MakeAvailable(SQLite3MultipleCiphers)
else()
    find_package(SQLite3 REQUIRED)
endif()

include_directories(SQLBridge/include/sqlcppbridge)
add_library ( lib::SQLCppBridge ALIAS ${PROJECT_NAME} )

target_compile_features ( ${PROJECT_NAME}
    PRIVATE  cxx_std_17 )

target_include_directories ( ${PROJECT_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/SQLBridge/include
    PUBLIC ${SQLite3_SOURCE_DIR}
)

set_target_properties( ${PROJECT_NAME} PROPERTIES
  FRAMEWORK ${BUILD_FRAMEWORK}
  FRAMEWORK_VERSION C
  VERSION           ${project_version}
  CXX_STANDARD      17
  PUBLIC_HEADER     "${HEADERS}" )

list (APPEND PUBLIC_HEADER "sqlite3.h sqlite3ext.h sqlite3mc_version.h sqlite3mc_vfs.h sqlite3mc.h")

if ( FETCH_SQLITE )
  
  if ( BUILD_SHARED )
    add_dependencies(${PROJECT_NAME} sqlite3mc)
    target_link_libraries (${PROJECT_NAME} sqlite3mc) # link right into the shared library
  else()
    add_dependencies(${PROJECT_NAME} sqlite3mc_static)
    target_link_libraries (${PROJECT_NAME} PUBLIC sqlite3mc_static) # just set as requirement for clients
  endif()
endif()

# installation
install ( TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include/sqlcppbridge)

cmake_minimum_required ( VERSION 3.15...3.16.2 )

set( CMAKE_PROJECT_INCLUDE_BEFORE 
  "${CMAKE_CURRENT_LIST_DIR}/common-project-include.in" )

project ( SQLCppBridge 
  VERSION      "${project_version}"
  DESCRIPTION  "${project_description}"
  HOMEPAGE_URL "${project_homepage}"
  LANGUAGES    C CXX )

include(ExternalProject)
set(SQLITE_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/sqlite)
set(SQLITE_SCRIPT_FILE ${CMAKE_CURRENT_LIST_DIR}/Support/sqlite/CMakeLists.txt)

ExternalProject_Add(sqlite
    URL https://www.sqlite.org/2020/sqlite-amalgamation-3330000.zip
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${SQLITE_INSTALL_LOCATION}
    PATCH_COMMAND ${CMAKE_COMMAND} -E copy "${SQLITE_SCRIPT_FILE}" "."
)

# possible options
option ( BUILD_SHARED    "Build shared library instead of static" OFF )
option ( BUILD_FRAMEWORK "Build framework" OFF )
option ( REQUIRE_SQLITE  "Link against sqlite3" ON )

# if the user specified a platform to build for (i.e. OS)
# we assume the user wants a framework
get_property( PLATFORM_SPECIFIED 
  VARIABLE PROPERTY PLATFORM  
  SET )

if ( PLATFORM_SPECIFIED )  
  set ( BUILD_FRAMEWORK ON )
endif()

file ( GLOB_RECURSE ALL_SRC "SQLBridge/src/*.cpp" )
file ( GLOB_RECURSE HEADERS "SQLBridge/include/*.h" )

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
MESSAGE ( STATUS "Build require sqlite:   " ${REQUIRE_SQLITE} )
MESSAGE ( STATUS "----" )

if ( REQUIRE_SQLITE )
    include_directories(${SQLITE_INSTALL_LOCATION}/include)
    add_dependencies(${PROJECT_NAME} sqlite)
endif()

add_library ( lib::SQLCppBridge ALIAS ${PROJECT_NAME} )

target_compile_features ( ${PROJECT_NAME}
    PRIVATE  cxx_std_14 )

target_include_directories ( ${PROJECT_NAME}
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/SQLBridge/src" 
    PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/SQLBridge/include"
)

set_target_properties( ${PROJECT_NAME} PROPERTIES
  FRAMEWORK ${BUILD_FRAMEWORK}
  FRAMEWORK_VERSION C
  VERSION           ${project_version}
  SOVERSION         1.0.0
  CXX_STANDARD      14
  PUBLIC_HEADER     "${HEADERS}" )

if ( REQUIRE_SQLITE )
#  find_package ( SQLite3 )

  if ( BUILD_SHARED )
    target_link_libraries (
      ${PROJECT_NAME}
        PRIVATE sqlite ) # link right into the shared library
  else()
    target_link_libraries (
      ${PROJECT_NAME}
        INTERFACE sqlite ) # just set as requirement for clients
  endif()
endif()

# installation
install ( TARGETS ${PROJECT_NAME} ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include/sqlcppbridge)
install (DIRECTORY ${SQLITE_INSTALL_LOCATION}/include DESTINATION include/sqlcppbridge)

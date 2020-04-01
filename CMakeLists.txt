cmake_minimum_required ( VERSION 3.15...3.16.2 )

set( CMAKE_PROJECT_INCLUDE_BEFORE 
  "${CMAKE_CURRENT_LIST_DIR}/common-project-include.in" )

project ( SQLCppBridge 
  VERSION      "${project_version}"
  DESCRIPTION  "${project_description}"
  HOMEPAGE_URL "${project_homepage}"
  LANGUAGES    C CXX )

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
  PUBLIC_HEADER     "${HEADERS}" )

if ( REQUIRE_SQLITE ) 
  find_package ( SQLite3 )

  if ( BUILD_SHARED )
    target_link_libraries (
      ${PROJECT_NAME}
        PRIVATE SQLite::SQLite3 ) # link right into the shared library
  else()
    target_link_libraries (
      ${PROJECT_NAME}
        INTERFACE SQLite::SQLite3 ) # just set as requirement for clients
  endif()
endif()

# installation
install ( TARGETS ${PROJECT_NAME} DESTINATION lib )
install ( FILES ${HEADERS} DESTINATION include )

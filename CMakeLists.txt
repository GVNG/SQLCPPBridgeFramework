cmake_minimum_required ( VERSION 3.15...3.16.2 )

set( CMAKE_PROJECT_INCLUDE_BEFORE 
  "${CMAKE_CURRENT_LIST_DIR}/common-project-include.in" )

project ( SQLCppBridge 
          VERSION "${project_version}"
          DESCRIPTION "${project_description}"
          HOMEPAGE_URL "${project_homepage}"
          LANGUAGES C CXX )

# possible options
option ( BUILD_SHARED    "Build shared library instead of static" OFF )
option ( BUILD_FRAMEWORK "Build framework" OFF )
option ( REQUIRE_SQLITE  "Link against sqlite3" ON )

file ( GLOB_RECURSE ALL_SRC "SQLBridge/src/*.cpp" )
file ( GLOB_RECURSE HEADERS "SQLBridge/include/*.hpp" )

if ( BUILD_SHARED )
  add_library ( ${PROJECT_NAME} SHARED "${ALL_SRC}" "${HEADERS}" )
  message(STATUS "Building shared version...")
else()
  add_library ( ${PROJECT_NAME} STATIC "${ALL_SRC}" "${HEADERS}" )
  message(STATUS "Building static version...")
endif()

add_library ( lib::SQLCppBridge ALIAS ${PROJECT_NAME} )

target_compile_features ( ${PROJECT_NAME}
    PRIVATE  cxx_std_14 )

target_include_directories ( ${PROJECT_NAME}
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/SQLBridge/src" "${CMAKE_CURRENT_SOURCE_DIR}/SQLBridge/include"
)

if ( REQUIRE_SQLITE ) 
  find_package ( SQLite3 )

  target_link_libraries (
    ${PROJECT_NAME}
      INTERFACE SQLite::SQLite3 )
endif()

# generate a framework for the above lib
if ( BUILD_FRAMEWORK )
  set_target_properties( ${PROJECT_NAME} PROPERTIES
    FRAMEWORK TRUE
    FRAMEWORK_VERSION C
    VERSION           ${project_version}
    SOVERSION         1.0.0
    PUBLIC_HEADER     "${HEADERS}" )
endif()

# installation
install ( TARGETS ${PROJECT_NAME} DESTINATION lib )
install ( FILES ${HEADERS} DESTINATION include )

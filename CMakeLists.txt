cmake_minimum_required (VERSION 3.15...3.16.2)

set( CMAKE_PROJECT_INCLUDE_BEFORE 
  "${CMAKE_CURRENT_LIST_DIR}/common-project-include.in" )

project( SQLCppBridge 
         VERSION "${project_version}"
         DESCRIPTION "${project_description}"
         HOMEPAGE_URL "${project_homepage}"
         LANGUAGES C CXX )

# set some project-wide flags
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-overflow -Wno-varargs -O3" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-overflow -Wno-varargs -O3" )

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-stdlib=libc++" COMPILER_SUPPORTS_LIBCPP)
CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)

if(COMPILER_SUPPORTS_CXX14)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
  if(COMPILER_SUPPORTS_LIBCPP)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
  endif()
else()
  message( STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++14 support. Please use a different C++ compiler." )
endif()

message( STATUS "CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS} )

# set includes
include_directories("SQLBridge/src" "SQLBridge/include")

# all source files
file(GLOB_RECURSE SOURCES SQLBridge/src/*.cpp)

# headers
file(GLOB_RECURSE HEADERS SQLBridge/include/*.h)

# lib
if(BUILD_SHARED)
  add_library (sqlcppbridge SHARED ${SOURCES} ${HEADERS})
  message(STATUS "Building shared version...")
else()
  add_library (sqlcppbridge STATIC ${SOURCES} ${HEADERS})
  message(STATUS "Building static version...")
endif()

# generate a framework for the above lib
set_target_properties( sqlcppbridge PROPERTIES
  FRAMEWORK TRUE
  FRAMEWORK_VERSION C
  VERSION ${project_version}
  SOVERSION 1.0.0
  PUBLIC_HEADER "${HEADERS}" )

# installation
install (TARGETS sqlcppbridge DESTINATION lib)
install (FILES ${HEADERS} DESTINATION include)

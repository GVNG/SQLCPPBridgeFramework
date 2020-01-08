cmake_minimum_required (VERSION 3.2)
project (sqlcppbridge C CXX)

# set some project-wide flags
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-overflow -Wno-varargs -O3" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-overflow -Wno-varargs -O3" )

# enable_testing()

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

# Make sure try_compile() works
#include(CheckTypeSize)
#check_type_size(time_t SIZEOF_TIME_T)

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

# installation
install (TARGETS sqlcppbridge DESTINATION lib)
install (FILES ${HEADERS} DESTINATION include)

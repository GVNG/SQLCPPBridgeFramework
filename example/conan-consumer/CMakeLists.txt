cmake_minimum_required(VERSION 3.16)
project(sqlcppbridge-consumer CXX)

find_package(SQLCppBridge REQUIRED)

add_executable(consumer main.cpp)
target_link_libraries(consumer PRIVATE SQLCppBridge::SQLCppBridge)
target_compile_features(consumer PRIVATE cxx_std_17)

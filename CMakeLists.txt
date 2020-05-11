cmake_minimum_required(VERSION 3.0.0)

project(isis_multicast VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(spdlog)
include_directories(spdlog/include include)

FILE(GLOB sources ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(isis_multicast ${sources})

target_link_libraries(isis_multicast
  spdlog
  boost_program_options
  boost_system
)

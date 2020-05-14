cmake_minimum_required(VERSION 3.0.0)

set(This isis_multicast)

project(${This} VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(spdlog)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  option(PACKAGE_TESTS "Build the tests" ON)
endif()

if(PACKAGE_TESTS)
  enable_testing()
  include(GoogleTest)
  add_subdirectory(tests)
endif()

find_package(Boost 1.67.0
  REQUIRED
  COMPONENTS system program_options
)

include_directories(spdlog/include include)

FILE(GLOB sources ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(${This} ${sources})
add_library(${This}_lib STATIC ${sources})

target_link_libraries(${This}
  spdlog
  ${Boost_LIBRARIES}
)


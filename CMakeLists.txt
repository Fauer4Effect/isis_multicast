# CMake build : global project

cmake_minimum_required (VERSION 3.10)

set(This isis_multicast)
project (${This} VERSION 0.1.0)

set_property (GLOBAL PROPERTY USE_FOLDERS ON)

set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost 1.67.0
  REQUIRED
  COMPONENTS system program_options
)
set (THREADS_PREFER_PTHREAD_FLAG ON)
find_package (Threads REQUIRED)

add_subdirectory (extern EXCLUDE_FROM_ALL)
add_subdirectory (lib)
add_subdirectory (app)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  option(PACKAGE_TESTS "Build the tests" ON)
endif()

if(PACKAGE_TESTS)
  enable_testing()
endif()
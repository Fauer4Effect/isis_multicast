set(BINARY ${CMAKE_PROJECT_NAME}_test)

add_subdirectory("${PROJECT_SOURCE_DIR}/googletest" "googletest")

include_directories(
  "${PROJECT_SOURCE_DIR}/spdlog/include"
  "${PROJECT_SOURCE_DIR}/include"
)

FILE(GLOB TEST_SOURCES *.cpp)

set(SOURCES ${TEST_SOURCES})

add_executable(${BINARY} ${TEST_SOURCES})

add_test(
  NAME ${BINARY} COMMAND ${BINARY}
)

find_package(Boost 1.67.0
  REQUIRED
  COMPONENTS system program_options
)

target_link_libraries(${BINARY} PUBLIC 
  ${CMAKE_PROJECT_NAME}_lib
  gtest
  gtest_main
  ${Boost_LIBRARIES}
)
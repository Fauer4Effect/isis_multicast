# CMake build : main application test

#configure variables
set (APP_NAME "${PROJECT_NAME}App")
set (TEST_NAME "${APP_NAME}Test")

if(PACKAGE_TESTS)
  enable_testing ()
  add_test (NAME ${TEST_NAME} COMMAND ${APP_NAME} )
endif()
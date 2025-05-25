cmake_minimum_required(VERSION 3.30)
project(banking)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build tests" ON)

set(LIB_SOURCE banking/account.h banking/account.cpp
    banking/transaction.h banking/transaction.cpp)
add_library(banking STATIC ${LIB_SOURCE})
target_include_directories(banking PUBLIC banking)

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(third-party/gtest)
  add_executable(test0 tests/test0)
  target_link_libraries(test0 ${PROJECT_NAME} gtest_main gmock_main)
  add_test(NAME test0 COMMAND test0)
endif()

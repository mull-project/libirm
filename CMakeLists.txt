cmake_minimum_required(VERSION 3.7)

project(libirm
  VERSION 0.6.0
  LANGUAGES C CXX
)

set (PROJECT_DESCRIPTION "Low-level IR mutations for LLVM Bitcode")
set (PROJECT_HOMEPAGE_URL "https://github.com/mull-project/libirm")

include(build-system/build-system.cmake)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  include(CTest)
endif()

add_subdirectory(lib)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
  add_subdirectory(tests)
endif()

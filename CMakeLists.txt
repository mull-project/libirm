cmake_minimum_required(VERSION 3.7)

project(libirm
  VERSION 0.1.0
  LANGUAGES CXX
)

set (PROJECT_DESCRIPTION "Low-level IR mutations for LLVM Bitcode")
set (PROJECT_HOMEPAGE_URL "https://github.com/mull-project/libirm")

option(LIBIRM_BUILD_32_BITS "Enable 32 bits build" OFF)

include(build-system/build-system.cmake)

add_subdirectory(lib)
add_subdirectory(tests)
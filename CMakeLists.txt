cmake_minimum_required(VERSION 3.15.0)
project(EoDAnalyzer VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

add_executable(EoDAnalyzer main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

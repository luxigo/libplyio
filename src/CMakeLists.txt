cmake_minimum_required (VERSION 3.1)
project (plyio)

add_subdirectory(tinyply)

# Get the current working branch
execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )

# Get the latest abbreviated commit hash of the working branch
execute_process(
  COMMAND git log -1 --format=%h
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )

set (plyio_VERSION_MAJOR 1)
set (plyio_VERSION_MINOR 0)
set (plyio_VERSION_PATCH 0)
set (plyio_GIT_BRANCH "\"${GIT_BRANCH}\"")
set (plyio_GIT_COMMIT "\"${GIT_COMMIT}\"")

configure_file (
  "${PROJECT_SOURCE_DIR}/ply_io.h.in"
  "${PROJECT_BINARY_DIR}/ply_io.h"
  )

include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/tinyply/source")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O")

include_directories(./tinyply)

add_library(plyio STATIC ply_io.cpp)
add_executable(examplyio ./example/example.cpp)
target_link_libraries(examplyio tinyply plyio)
set_property(TARGET examplyio PROPERTY CXX_STANDARD 11)
set_property(TARGET plyio PROPERTY CXX_STANDARD 11)

install(TARGETS plyio DESTINATION /usr/lib)

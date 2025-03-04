cmake_minimum_required(VERSION 3.16)
project(SandyShoreShowdown)

if(NOT CMAKE_BUILD_TYPE)
  set(default_build_type "Debug")
  message(STATUS "Set the build type to `${default_build_type}` as none was specified.")
  set(CMAKE_BUILD_TYPE ${default_build_type} CACHE STRING "Chooce the build type." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
  "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()
message(STATUS "SandyShoreShowdown Build Type: ${CMAKE_BUILD_TYPE}")

# Set the version for SandyShoreShowdown
set(SandyShoreShowdown_Version_Major 0)
set(SandyShoreShowdown_Version_Minor 3)
set(SandyShoreShowdown_Version_Patch 0)
set(SandyShoreShowdown_Version_Status "-dev")
set(PROJECT_VERSION
  "${SandyShoreShowdown_Version_Major}.${SandyShoreShowdown_Version_Minor}.${SandyShoreShowdown_Version_Patch}${SandyShoreShowdown_Version_Status}"
)
message(STATUS "${PROJECT_NAME} version: ${PROJECT_VERSION}")

add_executable(gameTest
              ${PROJECT_SOURCE_DIR}/src/test.c)

target_include_directories(gameTest 
                           PUBLIC ${PROJECT_SOURCE_DIR}/raylib/src/)
target_link_libraries(gameTest raylib GL m pthread dl rt X11)

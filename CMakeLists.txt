cmake_minimum_required(VERSION 3.2)
project(lobby)

if( NOT CMAKE_BUILD_TYPE )
  set(CMAKE_BUILD_TYPE "DEBUG")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/macros" "${CMAKE_SOURCE_DIR}/opt")

include(GroupSources)
include(AutoCollect)

if(WIN32)
  set(sources_windows
    ${CMAKE_SOURCE_DIR}/common/debug/WheatyExceptionReport.cpp
    ${CMAKE_SOURCE_DIR}/common/debug/WheatyExceptionReport.h
  )
endif(WIN32)

include(CheckCXXSourceRuns)
include(CheckIncludeFiles)
include(ConfigureBaseTargets)
include(CheckPlatform)
add_subdirectory(opt)
add_subdirectory(common)
add_subdirectory(otter)

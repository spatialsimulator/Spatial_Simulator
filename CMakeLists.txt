cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
message(STATUS "Running cmake version ${CMAKE_VERSION}")

project(spatialsimulator CXX)
set(LIB_VERSION_MAJOR 1)
set(LIB_VERSION_MINOR 0)
set(LIB_VERSION_PATCH 0)
set(LIB_VERSION ${LIB_VERSION_MAJOR}.${LIB_VERSION_MINOR}.${LIB_VERSION_PATCH})
set(LIB_COMPAT_VERSION ${LIB_VERSION_MAJOR}.${LIB_VERSION_MINOR})
ENABLE_LANGUAGE(C)
set(CMAKE_VERBOSE_MAKEFILE ON)

#variables
set(SPATIALSIM_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(SPATIALSIM_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
set(MAIN_FILES ${SPATIALSIM_SOURCE_DIR}/main.cpp)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

#cmake modules
include(cmake/find_libsbml.cmake)
#include(cmake/cpplint.cmake)

#opencv
find_package(OpenCV 3 REQUIRED)
if(NOT ${OpenCV_FOUND})
  message(FATAL_ERROR "OpenCV 3 not found")
endif()

##HDF5
find_package(HDF5 COMPONENTS HL REQUIRED )
if(NOT ${HDF5_FOUND})
  message(FATAL_ERROR "HDF5 not found")
endif()


##headers
add_subdirectory(${SPATIALSIM_INCLUDE_DIR})
##sources
add_subdirectory(${SPATIALSIM_SOURCE_DIR})

########### Add uninstall target ###############
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)
ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake")

########### Create jar  ###############
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_create_jar.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_create_jar.cmake"
  IMMEDIATE @ONLY)
ADD_CUSTOM_TARGET(deploy
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_create_jar.cmake")

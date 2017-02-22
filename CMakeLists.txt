cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
message(STATUS "Running cmake version ${CMAKE_VERSION}")

project(spatialsimulator CXX)
set(PACKAGE_VERSION "1.0.0")
ENABLE_LANGUAGE(C)

#variables
set(SPATIALSIM_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(SPATIALSIM_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)


#cmake modules
include(cmake/find_libsbml.cmake)
#include(cmake/cpplint.cmake)

#opencv
find_package(OpenCV 3 REQUIRED)

##HDF5
include(FindHDF5)

#headers
add_subdirectory(${SPATIALSIM_INCLUDE_DIR})
#sources
add_subdirectory(${SPATIALSIM_SOURCE_DIR})

#executable
#FILE(GLOB_RECURSE HEADERS )
#add_executable(spatialsimulator main.cpp ${include_files})
#target_link_libraries(spatialsimulator spatialsim ${LIBSBML_LIBRARIES} ${OPENCV_LIBRARIES} ${HDF5_CXX_LIBRARIES})
#install(TARGETS spatialsimulator
#  RUNTIME DESTINATION bin
#  )

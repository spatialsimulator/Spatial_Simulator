cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

file(GLOB_RECURSE SPATIALSIM_SOURCES "${SPATIALSIM_SOURCE_DIR}/*.cpp")

# headers
include_directories(${SPATIALSIM_INCLUDE_DIR})
include_directories(${LIBSBML_INCLUDE_DIR})
include_directories(${HDF5_INCLUDE_DIRS})
include_directories(${OPENCV_INCLUDE_DIR})

# static library
if(NOT without-static)
  add_library(spatialsim-static STATIC ${SPATIALSIM_SOURCES})
  target_link_libraries(spatialsim-static ${LIBSBML_LIBRARIES} ${OPENCV_LIBRARIES} ${HDF5_CXX_LIBRARIES})
  install(TARGETS spatialsim-static
    ARCHIVE DESTINATION lib
    )
endif()

# shared library
if(NOT without-shared)
  add_library(spatialsim SHARED ${SPATIALSIM_SOURCES})
  target_link_libraries(spatialsim ${LIBSBML_LIBRARIES} ${OPENCV_LIBRARIES} ${HDF5_CXX_LIBRARIES})
  set_target_properties(spatialsim PROPERTIES VERSION "${PACKAGE_VERSION}" SOVERSION "${PACKAGE_COMPAT_VERSION}")
  install(TARGETS spatialsim
    LIBRARY DESTINATION lib
    )
endif()

cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

file(GLOB SPATIALSIM_SOURCES ${SPATIALSIM_SOURCE_DIR})
list(REMOVE_ITEM SPATIALSIM_SOURCES ${MAIN_FILE})
file(GLOB MAIN_FILES "${SPATIALSIM_SOURCE_DIR}/*.cpp")

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
    ARCHIVE DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    )
endif()

# shared library
if(NOT without-shared)
  add_library(spatialsim SHARED ${SPATIALSIM_SOURCES})
  target_link_libraries(spatialsim ${LIBSBML_LIBRARIES} ${OPENCV_LIBRARIES} ${HDF5_CXX_LIBRARIES})
  set_target_properties(spatialsim PROPERTIES VERSION "${LIB_VERSION}" SOVERSION "${LIB_VERSION_MAJOR}")
  install(TARGETS spatialsim
      LIBRARY DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
      )
endif()

# executable
add_executable(spatialsimulator-bin ${MAIN_FILE} )
target_link_libraries(spatialsimulator-bin ${LIBSBML_LIBRARIES} ${OPENCV_LIBRARIES} ${HDF5_CXX_LIBRARIES})
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
target_link_libraries(${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libspatialsim.dylib)
SET_TARGET_PROPERTIES(spatialsimulator-bin PROPERTIES OUTPUT_NAME spatialsimulator)
install(TARGETS spatialsimulator-bin
    RUNTIME DESTINATION bin
  )

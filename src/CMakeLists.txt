cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

file(GLOB SPATIALSIM_SOURCES "${SPATIALSIM_SOURCE_DIR}/*.cpp")
list(REMOVE_ITEM SPATIALSIM_SOURCES ${MAIN_FILES})
file(GLOB SPATIALSIM_HEADERS "${SPATIALSIM_INCLUDE_DIR}/*.h")
# headers
include_directories(${LIBSBML_INCLUDE_DIR})
include_directories(${HDF5_INCLUDE_DIRS})
include_directories(${OPENCV_INCLUDE_DIR})
include_directories(${SPATIALSIM_INCLUDE_DIR})

# static library
if(NOT without-static)
  add_library(spatialsimulator-static STATIC ${SPATIALSIM_SOURCES})
  target_link_libraries(spatialsimulator-static ${LIBSBML_LIBRARIES} ${OpenCV_LIBRARIES} ${HDF5_LIBRARIES})
  install(TARGETS spatialsimulator-static
    ARCHIVE DESTINATION lib
    )
endif()

# shared library
if(NOT without-shared)
  add_library(spatialsimulator SHARED ${SPATIALSIM_SOURCES} )
  target_link_libraries(spatialsimulator ${LIBSBML_LIBRARIES} ${OpenCV_LIBRARIES} ${HDF5_CXX_LIBRARIES})
  set_target_properties(spatialsimulator PROPERTIES VERSION "${LIB_VERSION}" SOVERSION "${LIB_COMPAT_VERSION}" )
  install(TARGETS spatialsimulator
    LIBRARY DESTINATION lib
    )
endif()

# executable
add_executable(spatialsimulator-bin ${MAIN_FILES} )
target_link_libraries(spatialsimulator-bin spatialsimulator)
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
SET_TARGET_PROPERTIES(spatialsimulator-bin PROPERTIES OUTPUT_NAME spatialsimulator LINKER_LANGUAGE CXX)
install(TARGETS spatialsimulator-bin
    RUNTIME DESTINATION bin
  )

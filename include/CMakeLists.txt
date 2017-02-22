cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

# installation: headers
file(GLOB_RECURSE include_files "${SPATIALSIM_INCLUDE_DIR}/*.h")
install(FILES ${include_files} DESTINATION include/spatialsim)

cmake_minimum_required(VERSION 3.10)
 project(cesil)
 enable_testing()
 if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
   include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
   conan_basic_setup(TARGETS)
 endif()
execute_process(
  COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_SOURCE_DIR} git describe
  OUTPUT_VARIABLE CESIL_VERSION
  OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "Version of build from git repo is: ${CESIL_VERSION}")
configure_file(${CMAKE_SOURCE_DIR}/version.in
  ${CMAKE_SOURCE_DIR}/include/cesil/version.h
  @ONLY
)
add_subdirectory(src)
add_subdirectory(test)
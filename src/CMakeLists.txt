project(cesil)
add_executable(${PROJECT_NAME} main.cpp cesil.cpp mnemonic.cpp util.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE
  #libraries
)
target_include_directories(cesil PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_compile_definitions(${PROJECT_NAME} PRIVATE
  $<$<CONFIG:Debug>:DEBUG=1>
  $<$<CONFIG:Release>:DEBUG=0>
)
target_compile_options(${PROJECT_NAME} PRIVATE
  -Wall
  -Wextra
)

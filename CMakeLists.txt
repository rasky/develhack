cmake_minimum_required(VERSION 3.5)

set(CMAKE_TOOLCHAIN_FILE cmake/devkitARM.cmake)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(devkitARM)
include(NDS)

project(develfighter)

add_definitions(-DARM9)

set(EXE_NAME game)

add_executable(${EXE_NAME}
  src/main.c)

include_directories(${NDS_INCLUDE_DIR})

target_link_libraries(${EXE_NAME} ${NDS9_LIBRARY})

set_target_properties(${EXE_NAME}
  PROPERTIES
  LINK_FLAGS -specs=ds_arm9.specs
  COMPILER_FLAGS "-mthumb -mthumb-interwork")

objcopy_file(${EXE_NAME})
ndstool_file(${EXE_NAME})

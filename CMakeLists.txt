cmake_minimum_required(VERSION 3.5)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/Modules)
find_package(DevkitPRO REQUIRED)
project(develfighter C ASM)

add_definitions(-DARM9)

set(EXE_NAME game)

grit(gfx/starField.png)

include_directories(${NDS_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_executable(${EXE_NAME}
  src/main.c
  starField.s
  starField.h)

target_link_libraries(${EXE_NAME} ${NDS9_LIBRARY})

set_target_properties(${EXE_NAME}
  PROPERTIES
  LINK_FLAGS -specs=ds_arm9.specs
  COMPILER_FLAGS "-mthumb -mthumb-interwork")

objcopy_file(${EXE_NAME})
ndstool_file(${EXE_NAME})

cmake_minimum_required(VERSION 3.5)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/Modules)
find_package(DevkitPRO REQUIRED)
project(develfighter C ASM)

set(PROJ_C_FLAGS "-Os -mthumb -mthumb-interwork -specs=ds_arm9.specs")
set(CMAKE_C_FLAGS ${PROJ_C_FLAGS})
set(CMAKE_C_FLAGS_DEBUG ${PROJ_C_FLAGS})
set(CMAKE_C_FLAGS_RELEASE ${PROJ_C_FLAGS})

add_definitions(-DARM9)

#
# Game Binary
#

set(EXE_NAME game)

grit(gfx/starField.png)

include_directories(${NDS_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_executable(${EXE_NAME}
  src/main.c
  starField.s
  starField.h)

target_link_libraries(${EXE_NAME} ${NDS_FAT_LIBRARY} ${NDS9_LIBRARY})

objcopy_file(${EXE_NAME})
ndstool_file(${EXE_NAME})

#
# Data Disk
#

add_custom_command(
    TARGET ${EXE_NAME}
    POST_BUILD
    COMMAND ${CMAKE_SOURCE_DIR}/script/mkdatadisk
    ARGS ${CMAKE_SOURCE_DIR}/data ${CMAKE_CURRENT_BINARY_DIR}/data.bin)

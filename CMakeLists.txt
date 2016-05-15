cmake_minimum_required(VERSION 3.5)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/Modules)
find_package(DevkitPRO REQUIRED)
project(develfighter C ASM)

set(PROJ_C_FLAGS "-Os -mthumb -mthumb-interwork -specs=ds_arm9.specs")
set(CMAKE_C_FLAGS ${PROJ_C_FLAGS})
set(CMAKE_C_FLAGS_DEBUG ${PROJ_C_FLAGS})
set(CMAKE_C_FLAGS_RELEASE ${PROJ_C_FLAGS})

# Generate map file
set(CMAKE_EXE_LINKER_FLAGS "-Wl,-Map,game.map")

add_definitions(-DARM9)

#
# Lua
#

add_subdirectory(3rdparty/lua-5.3.2)
include_directories(3rdparty/lua-5.3.2/src)

find_package(Luac REQUIRED)

set(LUA_FILES ${CMAKE_CURRENT_SOURCE_DIR}/lua/hello.lua)
set(LUA_LIBRARIES lua ${MATH_LIBRARY})
set(LUA_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/luac.out)

#
# Game Binary
#

set(EXE_NAME game)

grit(gfx/starField.png)

include_directories(${NDS_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_executable(${EXE_NAME}
  src/debug.c
  src/input.c
  src/main.c
  src/anim.c
  src/frames.c
  starField.s)

target_link_libraries(${EXE_NAME} ${NDS_FAT_LIBRARY} ${NDS9_LIBRARY} ${LUA_LIBRARIES})

objcopy_file(${EXE_NAME})
ndstool_file(${EXE_NAME})

#
# Data Disk
#

# Pre-compile Lua files
add_custom_command(
  TARGET ${EXE_NAME}
  POST_BUILD
  DEPENDS ${LUA_FILES}
  COMMAND ${LUAC_EXE} -o ${LUA_OUTPUT} -- ${LUA_FILES})

# Create data disk
add_custom_command(
  TARGET ${EXE_NAME}
  POST_BUILD
  COMMAND ${CMAKE_SOURCE_DIR}/script/mkdatadisk
  ARGS ${CMAKE_CURRENT_BINARY_DIR}/data.bin ${CMAKE_CURRENT_SOURCE_DIR}/data/*.* ${LUA_OUTPUT})

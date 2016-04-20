#
# Toolchain
#

set(DEVKITARM $ENV{DEVKITARM})
set(DEVKITPRO $ENV{DEVKITPRO})

if(NOT DEVKITPRO)
  message(FATAL_ERROR "Please set DEVKITPRO in your environment")
endif()

if(NOT DEVKITARM)
  message(FATAL_ERROR "Please set DEVKITARM in your environment")
endif()

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm-none-eabi)
set(CMAKE_SYSTEM_VERSION 1)

set(CMAKE_C_COMPILER ${DEVKITARM}/bin/arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER ${DEVKITARM}/bin/arm-none-eabi-g++)
set(CMAKE_FIND_ROOT_PATH ${DEVKITPRO})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_FIND_LIBRARY_PREFIXES lib)
set(CMAKE_FIND_LIBRARY_SUFFIXES .a)

#
# libnds
#

find_path(NDS_INCLUDE_DIR
  NAMES nds.h
  HINTS "${DEVKITPRO}/libnds/include")

if(NOT NDS_INCLUDE_DIR)
  message(FATAL_ERROR "Unable to find nds.h")
endif()

find_library(NDS9_LIBRARY
  NAMES nds9 nds9d
  HINTS "${DEVKITPRO}/libnds/lib")

if(NOT NDS9_LIBRARY)
  message(FATAL_ERROR "Unable to find libnds for ARM9")
endif()

find_library(NDS_FAT_LIBRARY
  NAME fat
  HINTS "${DEVKITPRO}/libnds/lib")

#
# Tools
#

# grit
find_program(GRIT_EXE grit ${DEVKITARM}/bin)

if(NOT GRIT_EXE)
  message(FATAL_ERROR "Unable to find grit")
else()
  message(STATUS "Found grit -- ${GRIT_EXE}")
endif()

# ndstool
find_program(NDSTOOL_EXE ndstool ${DEVKITARM}/bin)

if(NOT NDSTOOL_EXE)
  message(FATAL_ERROR "Unable to find ndstool")
else()
  message(STATUS "Found ndstool -- ${NDSTOOL_EXE}")
endif()

#
# Macros
#

macro(GRIT PNG_FILE)
  make_directory(${CMAKE_CURRENT_BINARY_DIR})

  get_filename_component(BASE "${PNG_FILE}" NAME_WE)

  set(FO ${CMAKE_CURRENT_BINARY_DIR}/${BASE}.s ${CMAKE_CURRENT_BINARY_DIR}/${BASE}.h)
  set(FI ${CMAKE_CURRENT_SOURCE_DIR}/${PNG_FILE})

  add_custom_command(
    OUTPUT ${FO}
    COMMAND grit
    ARGS ${FI} -fts -o${CMAKE_CURRENT_BINARY_DIR}/${BASE}
    MAIN_DEPENDENCY ${FI})

  get_directory_property(extra_clean_files ADDITIONAL_MAKE_CLEAN_FILES)

  set_directory_properties(
    PROPERTIES
    ADDITIONAL_MAKE_CLEAN_FILES "${extra_clean_files};${FO}")

  set_source_files_properties(${FO} PROPERTIES GENERATED TRUE)
endmacro()

macro(NDSTOOL_FILE EXE_NAME)
  set(FO ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}.nds)
  set(I9 ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}.bin)

  add_custom_command(
    OUTPUT ${FO}
    COMMAND ${NDSTOOL_EXE}
    ARGS -c ${FO} -9 ${I9}
    MAIN_DEPENDENCY ${I9})

  get_filename_component(TGT "${EXE_NAME}" NAME)

  add_custom_target("Target9_${TGT}" ALL DEPENDS ${FO} VERBATIM)

  get_directory_property(extra_clean_files ADDITIONAL_MAKE_CLEAN_FILES)

  set_directory_properties(
    PROPERTIES
    ADDITIONAL_MAKE_CLEAN_FILES "${extra_clean_files};${FO}")

  set_source_files_properties(${FO} PROPERTIES GENERATED TRUE)
endmacro()

macro(OBJCOPY_FILE EXE_NAME)
  set(FO ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}.bin)
  set(FI ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME})

  message(STATUS ${FO})

  add_custom_command(
    OUTPUT "${FO}"
    COMMAND ${CMAKE_OBJCOPY}
    ARGS -O binary ${FI} ${FO}
    DEPENDS ${FI})

  get_filename_component(TGT "${EXE_NAME}" NAME)

  add_custom_target("TargetObjCopy_${TGT}" ALL DEPENDS ${FO} VERBATIM)

  get_directory_property(extra_clean_files ADDITIONAL_MAKE_CLEAN_FILES)

  set_directory_properties(
    PROPERTIES
    ADDITIONAL_MAKE_CLEAN_FILES "${extra_clean_files};${FO}")

  set_source_files_properties("${FO}" PROPERTIES GENERATED TRUE)
endmacro()

set(DEVKITPRO_FOUND 1)

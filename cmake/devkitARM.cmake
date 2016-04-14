set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR arm-eabi)

set(DEVKITARM $ENV{DEVKITARM})
set(DEVKITPRO $ENV{DEVKITPRO})

if(NOT DEVKITARM)
  message(FATAL_ERROR "Please set DEVKITARM in your environment")
endif(NOT DEVKITARM)

set(CMAKE_C_COMPILER ${DEVKITARM}/bin/arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER ${DEVKITARM}/bin/arm-none-eabi-g++)
set(CMAKE_FIND_ROOT_PATH ${DEVKITPRO})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_FIND_LIBRARY_PREFIXES lib)
set(CMAKE_FIND_LIBRARY_SUFFIXES .a)

find_path(NDS_INCLUDE_DIR
  NAMES nds.h
  HINTS "${DEVKITPRO}/libnds/include")

if(NOT NDS_INCLUDE_DIR)
  message(FATAL_ERROR "Unable to find nds.h")
endif(NOT NDS_INCLUDE_DIR)

find_library(NDS9_LIBRARY
  NAMES nds9 nds9d
  HINTS "${DEVKITPRO}/libnds/lib")

if(NOT NDS9_LIBRARY)
  message(FATAL_ERROR "Unable to find libnds for ARM9")
endif(NOT NDS9_LIBRARY)

find_library(NDS7_LIBRARY
  NAMES nds7 nds7d
  HINTS "${DEVKITPRO}/libnds/lib")

if(NOT NDS7_LIBRARY)
  message(FATAL_ERROR "Unable to find libnds for ARM7")
endif(NOT NDS7_LIBRARY)

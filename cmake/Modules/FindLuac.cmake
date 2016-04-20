find_program(LUAC_EXE NAMES luac luac5.3)

if(NOT LUAC_EXE)
  message(FATAL_ERROR "Cannot find luac")
else()
  message(STATUS "Found luac -- ${LUAC_EXE}")
endif()

set(LUA_FOUND 1)

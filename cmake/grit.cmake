set(GRIT_GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR})
make_directory(${GRIT_GENERATED_DIR})

macro(GRIT PNG_FILE)
  get_filename_component(BASE "${PNG_FILE}" NAME_WE)
  message(${BASE})
  set(FO ${GRIT_GENERATED_DIR}/${BASE}.s ${GRIT_GENERATED_DIR}/${BASE}.h)
  set(FI ${CMAKE_CURRENT_SOURCE_DIR}/${PNG_FILE})
  add_custom_command(
    OUTPUT ${FO}
    COMMAND grit
    ARGS ${FI} -fts -o${GRIT_GENERATED_DIR}/${BASE}
    MAIN_DEPENDENCY ${FI}
  )
  # add_custom_target("grit_${BASE}" ALL DEPENDS ${FO} VERBATIM)
  get_directory_property(extra_clean_files ADDITIONAL_MAKE_CLEAN_FILES)
  set_directory_properties(
    PROPERTIES
    ADDITIONAL_MAKE_CLEAN_FILES "${extra_clean_files};${FO}")
  set_source_files_properties(${FO} PROPERTIES GENERATED TRUE)
endmacro(GRIT)

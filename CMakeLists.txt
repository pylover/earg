set(sources
  "earg.c"
)



idf_component_register(
  SRCS "${sources}"
  INCLUDE_DIRS "include"
  REQUIRES 
)


target_compile_options(${COMPONENT_LIB} PRIVATE -fms-extensions)


set(sources
  "arghint.c"
  "builtin.c"
  "cmdstack.c"
  "command.c"
  "earg.c"
  "help.c"
  "option.c"
  "optiondb.c"
  "tokenizer.c"
)



idf_component_register(
  SRCS "${sources}"
  INCLUDE_DIRS "include"
  REQUIRES elog
)


target_compile_options(${COMPONENT_LIB} PRIVATE -fms-extensions)


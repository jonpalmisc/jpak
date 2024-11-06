cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(
  jpak
  VERSION 0.1.0
  LANGUAGES C)

add_library(jpak SHARED lib/jpak.c)
target_include_directories(jpak PUBLIC include)
target_compile_features(jpak PUBLIC c_std_99)

add_executable(jpak_cli src/main.c src/io.c)
set_target_properties(jpak_cli PROPERTIES OUTPUT_NAME jpak)
target_link_libraries(jpak_cli PRIVATE jpak)

install(TARGETS jpak jpak_cli)

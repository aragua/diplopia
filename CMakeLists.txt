cmake_minimum_required(VERSION 2.6)
project(diplopia)

set(CMAKE_C_FLAGS "-Wall -Werror -g -O0")

set(LIBRARY_OUTPUT_PATH lib)
set(EXECUTABLE_OUTPUT_PATH bin)
include_directories(include)

file(
  GLOB_RECURSE
  source_files
  src/*
  include/*
  )

file(
  GLOB_RECURSE
  include_files
  include/*.h
  )

add_library(
  diplopia
  SHARED
  ${source_files}
  )

add_library(
  diplopia-static
  STATIC
  ${source_files}
  )

add_executable(
  md5-test
  test/md5_test.c src/md5.c include/md5.h
  )

add_executable(
  list-test
  test/list_test.c include/list.h
  )

add_executable(
  diplopia-utils
  utils/diplopia-utils.c
  )

target_link_libraries(
  diplopia-utils
  diplopia
  )

install(
  TARGETS diplopia
  DESTINATION lib
  COMPONENT libraries
  )


install(FILES ${include_files}
  DESTINATION include/
  COMPONENT headers)

install(TARGETS diplopia-utils
  DESTINATION bin
  COMPONENT executables )

set(CPACK_GENERATOR TGZ)
set(CPACK_PACKAGE_NAME "diplopia")
include(CPack)
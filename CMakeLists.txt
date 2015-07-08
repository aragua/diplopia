cmake_minimum_required(VERSION 2.6)
project(diplopia)

set(CMAKE_C_FLAGS "-Wall -Werror -g -O0")

set(LIBRARY_OUTPUT_PATH lib)
set(EXECUTABLE_OUTPUT_PATH bin)
include_directories(include)

add_executable(
  md5-test
  test/md5_test.c src/md5.c include/md5.h
  )

add_executable(
  list-test
  test/list_test.c include/list.h
  )

add_executable(
  diplopia
  src/diplopia.c src/md5.c src/filesystem.c include/
  )

install(TARGETS diplopia
  DESTINATION bin
  COMPONENT executables )

set(CPACK_GENERATOR TGZ)
set(CPACK_PACKAGE_NAME "diplopia")
include(CPack)
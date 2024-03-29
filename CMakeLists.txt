project(FILTERING_NFA CXX)

cmake_minimum_required(VERSION 3.1)
set(BUFFER_SIZE "10" CACHE STRING "Size of buffer in Moore machine")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -march=native -DBUFFER_SIZE=${BUFFER_SIZE}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-deprecated-register")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-flto -O3 -DRELEASE")

# find_package(BISON REQUIRED)
# find_package(FLEX REQUIRED)
find_package(Boost REQUIRED COMPONENTS
  program_options unit_test_framework iostreams graph)
find_package(Eigen3 REQUIRED)

include_directories(
  include/
  ${PROJECT_BINARY_DIR}
  ${Boost_INCLUDE_DIRS})

## Config for Main filter
add_executable(filt
  src/main.cc
  src/timed_moore_machine.cc)

target_link_libraries(filt
#  profiler
 ${Boost_PROGRAM_OPTIONS_LIBRARY}
 ${Boost_GRAPH_LIBRARY})

target_include_directories(filt
  PRIVATE
  .
  ${CMAKE_CURRENT_BINARY_DIR})
set_property(TARGET filt
  PROPERTY CXX_STANDARD 17)

## Config for Construct only version
add_executable(construct_only
  src/construct_only.cc
  src/timed_moore_machine.cc)

target_link_libraries(construct_only
#  profiler
 ${Boost_PROGRAM_OPTIONS_LIBRARY}
 ${Boost_GRAPH_LIBRARY})

target_include_directories(construct_only
  PRIVATE
  .
  ${CMAKE_CURRENT_BINARY_DIR})
set_property(TARGET construct_only
  PROPERTY CXX_STANDARD 17)

## Config for Test
enable_testing()

add_executable(unit_test
  src/timed_moore_machine.cc
  test/unit_test.cc
  test/moore_machine_test.cc
  test/add_counter_test.cc
  test/automata_test.cc
  test/timed_moore_machine_test.cc
  test/dbm_test.cc
  test/bit_buffer_test.cc)

set_property(TARGET unit_test
  PROPERTY CXX_STANDARD 17)

target_link_libraries(unit_test
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_test(NAME unit_test
  COMMAND $<TARGET_FILE:unit_test>
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

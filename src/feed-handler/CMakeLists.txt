cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_COMPILER g++-14)
project(feed-handler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

find_package(Threads REQUIRED)

add_executable(feed-handler main.cpp)
target_compile_options(feed-handler PRIVATE -Wall -Wextra)
target_link_libraries(feed-handler PRIVATE Threads::Threads)

enable_testing()

add_executable(spsc-regression-test claude/spsc-regression-test.cpp)
target_compile_options(spsc-regression-test PRIVATE -Wall -Wextra -Wshadow)
add_test(NAME spsc-regression COMMAND spsc-regression-test)

#Run it on every build. A failing check fails the build.
add_custom_command(TARGET spsc-regression-test POST_BUILD
   COMMAND spsc-regression-test
   COMMENT "Running SPSC regression test")


add_executable(spsc-stress-test claude/spsc-stress-test.cpp)
target_compile_options(spsc-stress-test PRIVATE -Wall -Wextra -Wshadow -fsanitize=thread -O1 -g)
target_link_options(spsc-stress-test PRIVATE -fsanitize=thread)
add_test(NAME spsc-stress 
  COMMAND setarch ${CMAKE_SYSTEM_PROCESSOR} -R $<TARGET_FILE:spsc-stress-test>)

#Run it on every build. A failing check fails the build.
# add_custom_command(TARGET spsc-stress-test POST_BUILD
#    COMMAND spsc-stress-test
#    COMMENT "Running SPSC stress test")

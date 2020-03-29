# Project mm7 main cmake file

cmake_minimum_required(VERSION 3.1)

project(mm7 VERSION 0.0.1)

include( CheckCSourceCompiles )

# Option to install targets to env location
if(DEFINED ENV{MM7_HOME})
     message("-- Will install MM7_HOME binaries and libraries to location: " $ENV{MM7_HOME})
	 set(CMAKE_INSTALL_PREFIX $ENV{MM7_HOME})
else(DEFINED ENV{MM7_HOME})
     message("-- Will install MM7_HOME binaries and libraries to location: " ${CMAKE_CURRENT_BINARY_DIR})
	 set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR})
endif(DEFINED ENV{MM7_HOME})


# Disable some windows specific warnings for using more modern c functions
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif(MSVC)

# Options
option(WITH_testing "Allows tests to be run" ON)

if(WITH_testing MATCHES ON)
   enable_testing()
endif(WITH_testing MATCHES ON)

# Test if the core header compiles
file(READ mm7-core.h MM7_CORE_HEADER)
set(TEST_MM7_CORE_HEADER "${MM7_CORE_HEADER} \n\n int main(void) { return 0; }")
check_c_source_compiles("${TEST_MM7_CORE_HEADER}" WORKS_MM7_CORE_HEADER)

add_executable(mm7-tester mm7-tester.c)
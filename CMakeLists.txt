set(PROJECT amqp-tutorial)

message(STATUS "VCPKG_DIR: $ENV{VCPKG_DIR}")
if(DEFINED ENV{VCPKG_DIR} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
	set(CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_DIR}/scripts/buildsystems/vcpkg.cmake CACHE STRING "")
endif()
message(STATUS "CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
#include(c:/work/code/vcpkg/scripts/buildsystems/vcpkg.cmake)

project(${PROJECT} C CXX)
#cmake_minimum_required(VERSION 2.8.11)
cmake_minimum_required(VERSION 3.12...3.18)

IF(MSVC)
    add_definitions(-D_USE_MATH_DEFINES)
    add_definitions(-DNOMINMAX)
ENDIF()

set(CMAKE_CXX_STANDARD 11) 
IF (WIN32)
  # set stuff for windows
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
ELSE()
  # set stuff for other systems
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -pedantic")
ENDIF()

find_package(amqpcpp CONFIG REQUIRED)
find_package(Poco CONFIG REQUIRED Net XML Zip Data)
#target_link_libraries(main PRIVATE amqpcpp)

#include_directories(3rdparty/AMQP-CPP-2.1.4                  3rdparty/)
include_directories(3rdparty/)

#add_subdirectory(3rdparty/AMQP-CPP-2.1.4)
add_subdirectory(src)
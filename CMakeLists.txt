cmake_minimum_required(VERSION 2.6)
project(gremlins C CXX)

find_package(MPI)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmakefiles;${CMAKE_MODULE_PATH}")
find_package(Msr)
INCLUDE(WrapConfig)

include_directories(./utils)

ADD_SUBDIRECTORY(power)
ADD_SUBDIRECTORY(thermal)
ADD_SUBDIRECTORY(memory)
ADD_SUBDIRECTORY(resilience)

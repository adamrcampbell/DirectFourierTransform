cmake_minimum_required(VERSION 3.10.2)

# Base direct fourier transform project
project(dft)
add_executable(dft direct_fourier_transform.c)
target_link_libraries(dft m)

# Unit testing for dft
project(tests)
# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
add_executable(tests unit_testing.cpp)
target_link_libraries(tests ${GTEST_LIBRARIES} pthread)

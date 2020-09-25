cmake_minimum_required(VERSION 3.5)

set(EXTRA_COMPONENT_DIRS ../../EBLi)
set(TEST_COMPONENTS "EBLi" CACHE STRING "List of components to test")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(ebli_test)

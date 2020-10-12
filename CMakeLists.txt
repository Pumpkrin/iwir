cmake_minimum_required(VERSION 3.2)

project(iwir_project VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ROOT 6.10 CONFIG REQUIRED)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(RootNewMacros)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/)
set(CMAKE_INSTALL_LIBDIR ${CMAKE_BINARY_DIR}/lib/)

root_generate_dictionary( G__iwir iwir.hpp LINKDEF linkdef.h)

add_library( iwir SHARED iwir.cpp G__iwir.cxx saver.cpp configurator.cpp )
target_include_directories(iwir PUBLIC "${ROOT_INCLUDE_DIRS}")
target_link_libraries(iwir PUBLIC ROOT::Core ROOT::Hist ROOT::Gpad)


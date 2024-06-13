cmake_minimum_required (VERSION 3.5 FATAL_ERROR)

project(cpp-generic-toolbox)

# This toolbox is written in C++11
set (CMAKE_CXX_STANDARD 11)

# Include the header folder
include_directories(./include)

add_executable(GenericToolboxExample example/main.cpp)

if ( DEFINED ENV{ROOTSYS} )
    find_package(ROOT)
    message("ROOT Library has been found.")

    add_definitions( -D ENABLE_ROOT_EXTENSION )

    target_link_libraries(GenericToolboxExample ${ROOT_LIBRARIES})

    execute_process (COMMAND root-config --cflags OUTPUT_VARIABLE ROOT_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process (COMMAND root-config --libs OUTPUT_VARIABLE ROOT_LD_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process (COMMAND root-config --version OUTPUT_VARIABLE ROOT_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process (COMMAND root-config --incdir OUTPUT_VARIABLE ROOT_INCLUDE_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
    message("[ROOT]: root-config --version: ${ROOT_VERSION}")
    message("[ROOT]: root-config --cflags: ${ROOT_CXX_FLAGS}")
    message("[ROOT]: root-config --libs: ${ROOT_LD_FLAGS}")
    message("[ROOT]: root-config --incdir: ${ROOT_INCLUDE_DIR}")
    include_directories(${ROOT_INCLUDE_DIR})

    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${ROOT_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fPIC ${ROOT_CXX_FLAGS}")

    link_directories(${ROOT_LIBRARY_DIRS})
endif()


cmake_minimum_required(VERSION 3.10)

set(BOREALIS_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/library/borealis/library)


# build options
include(${BOREALIS_LIBRARY}/cmake/commonOption.cmake)

# Dependencies
option(USE_SHARED_LIB "Whether to use shared libs provided by system" OFF)
cmake_dependent_option(USE_SYSTEM_FMT "" OFF "NOT USE_SHARED_LIB" ON)
cmake_dependent_option(USE_SYSTEM_TINYXML2 "" OFF "NOT USE_SHARED_LIB" ON)
cmake_dependent_option(USE_SYSTEM_TWEENY "" OFF "NOT USE_SHARED_LIB" ON)

# toolchain
include(${BOREALIS_LIBRARY}/cmake/toolchain.cmake)

# project info
project(nso-icon-tool)
set(VERSION_MAJOR "0")
set(VERSION_MINOR "4")
set(VERSION_ALTER "3")
set(VERSION_BUILD "0")
set(PROJECT_AUTHOR "dslatt")
set(PROJECT_ICON ${CMAKE_CURRENT_SOURCE_DIR}/resources/img/dev.jpg)
set(PROJECT_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/resources)
set(VERSION_STRING ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_ALTER})
set(USE_LIBROMFS)

if (USE_LIBROMFS)
    add_libromfs(${PROJECT_NAME} ${PROJECT_RESOURCES})
endif ()

# build borealis
add_subdirectory(${BOREALIS_LIBRARY})

find_package(Threads REQUIRED)
list(APPEND APP_PLATFORM_LIB ${CMAKE_THREAD_LIBS_INIT})

# setting src and include
list(APPEND APP_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB_RECURSE MAIN_SRC source/*.cpp)

# external headers
list(APPEND EXTERNAL_INCLUDE_DIRS ${BOREALIS_LIBRARY}/include)
list(APPEND EXTERNAL_INCLUDE_DIRS ${BOREALIS_LIBRARY}/include/borealis)
list(APPEND EXTERNAL_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/library/headers)

set(PLATFORM_OPTION)
if (PLATFORM_SWITCH)
    set(APP_PLATFORM_LIB
        # needed by borealis
        glfw3 EGL glapi drm_nouveau
        # base lib
        nx m
        # extract
        archive
        # curl
        curl z bz2 zstd lzma lz4
        # hash
        xxhash
    )
    list(APPEND MAIN_SRC ${BOREALIS_LIBRARY}/lib/platforms/switch/switch_wrapper.c)
    list(APPEND EXTERNAL_INCLUDE_DIRS ${DEVKITPRO}/portlibs/switch/include)
endif ()

# building target
program_target(${PROJECT_NAME} "${MAIN_SRC}")

# include dirs; mark external as system for warning suppression
target_include_directories(${PROJECT_NAME} PRIVATE ${APP_INCLUDE})
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${EXTERNAL_INCLUDE_DIRS})

# c++23 not supported by current toolchain cmake version; use compiler flags instead
#set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)

# building release file
if (PLATFORM_SWITCH)
    if (USE_DEKO3D)
        gen_dksh("${PROJECT_RESOURCES}/shaders")
    endif ()
    add_custom_target(${PROJECT_NAME}.nro DEPENDS ${PROJECT_NAME}
        COMMAND ${NX_NACPTOOL_EXE} --create "${PROJECT_NAME}" "${PROJECT_AUTHOR}" "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_ALTER}" ${PROJECT_NAME}.nacp --titleid=${PROJECT_TITLEID}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_RESOURCES} ${CMAKE_BINARY_DIR}/resources
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/resources/font
        COMMAND ${NX_ELF2NRO_EXE} ${PROJECT_NAME}.elf ${PROJECT_NAME}.nro --icon=${PROJECT_ICON} --nacp=${PROJECT_NAME}.nacp --romfsdir=${CMAKE_BINARY_DIR}/resources
    )
endif ()

# version injection
set(PRE_CONFIGURE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/include/version.h.in")
set(POST_CONFIGURE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/include/version.h")
include(git_watcher.cmake)
CheckGit("${GIT_WORKING_DIR}" changed)
GitStateChangedAction()

# warnings
# list(APPEND APP_PLATFORM_OPTION -Wall -Wextra -Wno-unused-parameter)

target_compile_options(${PROJECT_NAME} PRIVATE -ffunction-sections -fdata-sections -std=c++2b  ${APP_PLATFORM_OPTION})
target_link_libraries(${PROJECT_NAME} PRIVATE borealis ${APP_PLATFORM_LIB})

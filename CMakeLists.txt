cmake_minimum_required(VERSION 3.22)

project(GameConsole C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

add_executable(${CMAKE_PROJECT_NAME})

add_subdirectory(cmake/stm32cubemx)

# Project folders
set(PROJECT_SOURCE_DIRS
    platform
    shared
    games
    game_1
    game_2
    game_3
    assets
)

# User source files
set(USER_SOURCES)

foreach(dir ${PROJECT_SOURCE_DIRS})
    if(EXISTS "${CMAKE_SOURCE_DIR}/${dir}")
        file(GLOB_RECURSE DIR_SOURCES CONFIGURE_DEPENDS
            "${CMAKE_SOURCE_DIR}/${dir}/*.c"
        )
        list(APPEND USER_SOURCES ${DIR_SOURCES})
    endif()
endforeach()

target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    ${USER_SOURCES}
)

# Include directories
set(USER_INCLUDE_DIRS
    "${CMAKE_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/assets"
)

foreach(dir ${PROJECT_SOURCE_DIRS})
    if(EXISTS "${CMAKE_SOURCE_DIR}/${dir}")
        file(GLOB_RECURSE DIR_HEADERS CONFIGURE_DEPENDS
            "${CMAKE_SOURCE_DIR}/${dir}/*.h"
        )

        foreach(header ${DIR_HEADERS})
            get_filename_component(header_dir "${header}" DIRECTORY)
            list(APPEND USER_INCLUDE_DIRS "${header_dir}")
        endforeach()
    endif()
endforeach()

list(REMOVE_DUPLICATES USER_INCLUDE_DIRS)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${USER_INCLUDE_DIRS}
)

# Compile definitions
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
)

# Link libraries
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
)

target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
    -u _printf_float
    # -u _scanf_float
)

# Generate binary output
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary
        $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
        ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.bin
    BYPRODUCTS
        ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.bin
    COMMENT "Generating .bin from ELF"
)
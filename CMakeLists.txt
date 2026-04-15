cmake_minimum_required(VERSION 3.22)

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Core project settings
project(GameConsole)

# Enable compile commands for clangd etc.
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Enable C and ASM
enable_language(C ASM)

# Create executable target
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# -----------------------------
# Auto-discover user source files
# -----------------------------
file(GLOB_RECURSE USER_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/platform/*.c"
    "${CMAKE_SOURCE_DIR}/shared/*.c"
    "${CMAKE_SOURCE_DIR}/game_1/*.c"
    "${CMAKE_SOURCE_DIR}/game_2/*.c"
    "${CMAKE_SOURCE_DIR}/game_3/*.c"
    "${CMAKE_SOURCE_DIR}/games/*.c"
)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    ${USER_SOURCES}
)

# -----------------------------
# Auto-discover include directories
# -----------------------------
set(USER_INCLUDE_DIRS
    "${CMAKE_SOURCE_DIR}"
)

function(collect_include_dirs base_dir)
    if(EXISTS "${base_dir}")
        file(GLOB_RECURSE found_headers CONFIGURE_DEPENDS
            "${base_dir}/*.h"
        )
        foreach(header ${found_headers})
            get_filename_component(header_dir "${header}" DIRECTORY)
            list(APPEND USER_INCLUDE_DIRS "${header_dir}")
        endforeach()
        set(USER_INCLUDE_DIRS "${USER_INCLUDE_DIRS}" PARENT_SCOPE)
    endif()
endfunction()

collect_include_dirs("${CMAKE_SOURCE_DIR}/platform")
collect_include_dirs("${CMAKE_SOURCE_DIR}/shared")
collect_include_dirs("${CMAKE_SOURCE_DIR}/game_1")
collect_include_dirs("${CMAKE_SOURCE_DIR}/game_2")
collect_include_dirs("${CMAKE_SOURCE_DIR}/game_3")
collect_include_dirs("${CMAKE_SOURCE_DIR}/games")

list(REMOVE_DUPLICATES USER_INCLUDE_DIRS)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${USER_INCLUDE_DIRS}
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user-defined symbols here if needed
)

# Remove wrong libob.a dependency when using some toolchains
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Link libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
)

# Enable floating point printf
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
    -u _printf_float
    #-u _scanf_float
)

# Create .bin from ELF
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary
        $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
        ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.bin
    BYPRODUCTS
        ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.bin
    COMMENT "Generating .bin from ELF"
)
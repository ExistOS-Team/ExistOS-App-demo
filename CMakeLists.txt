cmake_minimum_required(VERSION 3.16)

include(ExternalProject)

set(CMAKE_TOOLCHAIN_FILE toolchain.cmake)

project(EOSTestAPP VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)

SET(BUILD_SHARED_LIBRARIES ON)

add_link_options( -T ${CMAKE_SOURCE_DIR}/ld.script -Wl,--gc-sections -Wl,--unresolved-symbols=ignore-in-object-files  -Wl,--emit-relocs -shared )
 
include_directories(./)

aux_source_directory(.   APP_SRCS)

add_executable(testApp.elf ${APP_SRCS})

target_compile_options(testApp.elf PRIVATE -mtune=arm926ej-s -mcpu=arm926ej-s -mlittle-endian -marm -fpic -shared 
-O0 -pipe -fdata-sections -ffunction-sections -fcommon -finline-functions -Wall)

link_directories(${PROJECT_SOURCE_DIR}/lib)

target_link_libraries(testApp.elf PRIVATE -nostartfiles -nodefaultlibs -nostdlib)

add_custom_command(TARGET testApp.elf POST_BUILD
    COMMAND arm-none-eabi-strip --strip-debug ${CMAKE_CURRENT_BINARY_DIR}/testApp.elf 
    COMMAND arm-none-eabi-size ${CMAKE_CURRENT_BINARY_DIR}/testApp.elf 
)


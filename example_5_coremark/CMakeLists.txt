cmake_minimum_required(VERSION 3.16)

include(ExternalProject)

set(CMAKE_TOOLCHAIN_FILE toolchain.cmake)

project(CoreMark VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)

SET(BUILD_SHARED_LIBRARIES OFF)



add_link_options( -T ${CMAKE_SOURCE_DIR}/ld.script 
-nostartfiles -nodefaultlibs -nostdlib 
-Wl,--wrap=malloc -Wl,--wrap=free 
-Wl,--unresolved-symbols=ignore-in-object-files 
-Wl,--emit-relocs  -static 
-Wl,--gc-sections)

include_directories(./)
include_directories(./include)
include_directories(./include/lvgl) 
include_directories(./include/freertos)

include_directories(./coremark)

aux_source_directory(./coremark   APP_SRCS)

aux_source_directory(.   APP_SRCS)

add_executable(${CMAKE_PROJECT_NAME}.elf ${APP_SRCS})

target_compile_options(${CMAKE_PROJECT_NAME}.elf PRIVATE 
-mtune=arm926ej-s 
-mcpu=arm926ej-s 
-mlittle-endian 
-marm 
-mlong-calls
-O3
-pipe -fdata-sections -ffunction-sections -fcommon -finline-functions
)


add_custom_command(TARGET ${CMAKE_PROJECT_NAME}.elf POST_BUILD
    COMMAND arm-none-eabi-size ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.elf 
    COMMAND ${CMAKE_SOURCE_DIR}/tools/eld.exe --appelf ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.elf --symtab ${CMAKE_SOURCE_DIR}/sys_symtab.txt --exp ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.exp
)

add_custom_target(${CMAKE_PROJECT_NAME}.exp 
    COMMAND ${CMAKE_SOURCE_DIR}/tools/eld.exe --appelf ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.elf --symtab ${CMAKE_SOURCE_DIR}/sys_symtab.txt --exp ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.exp
    DEPENDS ${CMAKE_PROJECT_NAME}.elf
)

add_custom_target(Relocate
    COMMAND ${CMAKE_SOURCE_DIR}/tools/eld.exe --symtab ${CMAKE_SOURCE_DIR}/sys_symtab.txt --exp ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.exp
    DEPENDS ${CMAKE_PROJECT_NAME}.exp
)


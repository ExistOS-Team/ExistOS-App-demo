cmake_minimum_required(VERSION 3.16)

include(ExternalProject)

set(CMAKE_TOOLCHAIN_FILE toolchain.cmake)

project(EOSTestAPP VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)

SET(BUILD_SHARED_LIBRARIES OFF)

add_link_options( -T ${CMAKE_SOURCE_DIR}/ld.script -nostartfiles -nodefaultlibs -nostdlib -Wl,--wrap=malloc -Wl,--wrap=free -Wl,--unresolved-symbols=ignore-in-object-files -Wl,--emit-relocs  -static 
 -Wl,--gc-sections  )

include_directories(./)
include_directories(./RepicoGL)
include_directories(./include/freertos)
include_directories(./include)

aux_source_directory(.   APP_SRCS)

add_subdirectory(RepicoGL)

add_executable(testApp.elf ${APP_SRCS})

target_compile_options(picogl PRIVATE -mtune=arm926ej-s -mcpu=arm926ej-s -mlittle-endian -mthumb -mlong-calls
-O3 -pipe -fdata-sections -ffunction-sections -fcommon -finline-functions)

target_compile_options(testApp.elf PRIVATE -mtune=arm926ej-s -mcpu=arm926ej-s -mlittle-endian -marm -mlong-calls
-O3 -pipe -fdata-sections -ffunction-sections -fcommon -finline-functions)

target_link_libraries(testApp.elf PRIVATE 
picogl 
)

add_custom_command(TARGET testApp.elf POST_BUILD
    COMMAND arm-none-eabi-size ${CMAKE_CURRENT_BINARY_DIR}/testApp.elf 
)

add_custom_target(testApp.exp 
    COMMAND ${CMAKE_SOURCE_DIR}/tools/eld.exe --appelf ${CMAKE_CURRENT_BINARY_DIR}/testApp.elf --symtab ${CMAKE_SOURCE_DIR}/sys_symtab.txt --exp ${CMAKE_CURRENT_BINARY_DIR}/testapp.exp
    DEPENDS testApp.elf
)

add_custom_target(Relocate
    COMMAND ${CMAKE_SOURCE_DIR}/tools/eld.exe --symtab ${CMAKE_SOURCE_DIR}/sys_symtab.txt --exp ${CMAKE_CURRENT_BINARY_DIR}/testapp.exp
    DEPENDS testApp.exp
)


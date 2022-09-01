cmake_minimum_required(VERSION 3.16)

include(ExternalProject)

set(CMAKE_TOOLCHAIN_FILE toolchain.cmake)

project(EOSTestAPP VERSION 0.1.0)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)

SET(BUILD_SHARED_LIBRARIES OFF)

add_link_options(-T ${CMAKE_SOURCE_DIR}/ld.script -Wl,--gc-sections)

SET(CMAKE_EXE_LINKER_FLAGS "-static")
 
include_directories(./)

aux_source_directory(.   APP_SRCS)

add_executable(testApp.elf ${APP_SRCS})

target_link_libraries(testApp.elf PRIVATE -lm -lc -nostartfiles)

target_compile_options(testApp.elf PRIVATE -mtune=arm926ej-s -mcpu=arm926ej-s -mlittle-endian -mthumb
-Os -pipe -fdata-sections -ffunction-sections -fcommon -finline-functions -Wall)

add_custom_command(TARGET testApp.elf POST_BUILD
    COMMAND arm-none-eabi-size ${CMAKE_CURRENT_BINARY_DIR}/testApp.elf 
)

add_custom_target(
  testApp.bin ALL
  COMMAND ${CMAKE_OBJCOPY} -I elf32-little -O binary -R .note -R .comment -S ${CMAKE_CURRENT_BINARY_DIR}/testApp.elf ${CMAKE_CURRENT_BINARY_DIR}/testApp.bin 
  DEPENDS testApp.elf 
  )


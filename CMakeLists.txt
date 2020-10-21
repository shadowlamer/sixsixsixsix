cmake_minimum_required(VERSION 3.0)

set(CMAKE_TOOLCHAIN_FILE z80.cmake)

set(TARGET yrgb2020)
set(TARGET_VERSION "0.0.1")

set(MAIN_CODE_LOC 32768)
set(DATA_LOC 51440)

project(${TARGET})

set(CMAKE_C_FLAGS "-mz80 --no-std-crt0 --vc --code-loc ${MAIN_CODE_LOC} --data-loc ${DATA_LOC}  --std-sdcc99")

file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/*.c")
add_executable( ${TARGET} ${SOURCES})
add_custom_target(${TARGET}.bin
                   COMMAND srec_cat ${TARGET}.ihx --intel --offset -${MAIN_CODE_LOC} -o ${TARGET}.bin --binary
                   DEPENDS ${TARGET}
                   COMMENT "Making ${TARGET}.bin")
add_custom_target(${TARGET}.tap ALL 
                   DEPENDS ${TARGET}.bin
                   COMMAND bin2tap -o ${TARGET}.tap ${TARGET}.bin -b
                   COMMENT "Making ${TARGET}.tap")

include_directories(inc)

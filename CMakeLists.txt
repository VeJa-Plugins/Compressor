cmake_minimum_required(VERSION 3.5)

project(compressor
        VERSION 1.0.0
        LANGUAGES C
        )

#sharing is caring
set(SORAMA_SHARED TRUE)
# Set C 99
set(CMAKE_C_STANDARD 99)

add_library(
        compressor-core STATIC
        ${CMAKE_CURRENT_LIST_DIR}/first_order_lowpass.h
        ${CMAKE_CURRENT_LIST_DIR}/first_order_lowpass.c
        ${CMAKE_CURRENT_LIST_DIR}/compressor_core.h
        ${CMAKE_CURRENT_LIST_DIR}/compressor_core.c
)

add_executable(compressorMock compressorMock.c)

target_link_libraries(compressorMock compressor-core)

install(TARGETS compressorMock DESTINATION bin)
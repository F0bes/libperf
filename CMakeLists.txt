cmake_minimum_required(VERSION 3.10)

set(CMAKE_TOOLCHAIN_FILE $ENV{PS2SDK}/ps2dev.cmake)

project(perf)

enable_language(C ASM)

add_library(perf perf.c perf.h l2handle.s)

target_link_libraries(perf
	kernel
	)

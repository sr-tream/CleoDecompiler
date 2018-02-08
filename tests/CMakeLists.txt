cmake_minimum_required(VERSION 2.8)

option(BUILD_TEST_OPCODES "Build test opcodes [ON/OFF]" ON)
option(BUILD_TEST_DECOMPILE "Build test decompile [ON/OFF]" ON)


if (BUILD_TEST_OPCODES)
	add_subdirectory(opcodes)
endif(BUILD_TEST_OPCODES)

if (BUILD_TEST_DECOMPILE)
	add_subdirectory(decompile)
endif(BUILD_TEST_DECOMPILE)

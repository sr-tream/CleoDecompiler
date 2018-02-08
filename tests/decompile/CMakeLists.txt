cmake_minimum_required(VERSION 2.8)

option(BUILD_TEST_DECOMPILE_FIRST_OPCODE "Build test decompile first opcode [ON/OFF]" ON)
option(BUILD_TEST_DECOMPILE_ALL_OPCODE "Build test decompile all opcode [ON/OFF]" ON)


if (BUILD_TEST_DECOMPILE_FIRST_OPCODE)
	add_subdirectory(firstOpcode)
endif(BUILD_TEST_DECOMPILE_FIRST_OPCODE)

if (BUILD_TEST_DECOMPILE_ALL_OPCODE)
	add_subdirectory(allOpcode)
endif(BUILD_TEST_DECOMPILE_ALL_OPCODE)

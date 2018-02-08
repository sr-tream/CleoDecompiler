cmake_minimum_required(VERSION 2.8)

project(CleoDecompiler)

option(BUILD_TESTS "Build tests [ON/OFF]" OFF)


macro( getSrcList placeholder floderName )
	set( GSL "" )
	file(GLOB GSL_CPP "${floderName}/*.cpp")
	file(GLOB GSL_HPP "${floderName}/*.hpp")
	file(GLOB GSL_C "${floderName}/*.c")
	file(GLOB GSL_H "${floderName}/*.h")
	file(GLOB GSL ${GSL}}
		${GSL_CPP}
		${GSL_HPP}
		${GSL_C}
		${GSL_H})
endmacro( getSrcList )


set( GSL "" )
getSrcList(${GSL}, .)
file(GLOB ${PROJECT_NAME}_LIST ${${PROJECT_NAME}_LIST} ${GSL})

set( GSL "" )
getSrcList(${GSL}, FileMan)
file(GLOB ${PROJECT_NAME}_LIST ${${PROJECT_NAME}_LIST} ${GSL})


add_definitions("-std=c++14 -fexec-charset=CP1251")

add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_LIST})


if (BUILD_TESTS)
	add_subdirectory(tests)
endif(BUILD_TESTS)

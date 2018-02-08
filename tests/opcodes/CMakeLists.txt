cmake_minimum_required(VERSION 2.8)

project(test_opcodes)


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


file(GLOB LIB_CD_H "../../*.h")
file(GLOB LIB_CD_HPP "../../*.hpp")
file(GLOB LIB_CD_FMH "../../FileMan/*.h")
file(GLOB ${PROJECT_NAME}_LIST ${${PROJECT_NAME}_LIST}
	${LIB_CD_H}
	${LIB_CD_HPP}
	${LIB_CD_FMH})


add_definitions("-std=c++14 -ffunction-sections -fexec-charset=CP1251")
set(BUILD_LFLAGS "-Wl,--gc-sections -Wl,--strip-all")
set(CMAKE_SHARED_LINKER_FLAGS ${BUILD_LFLAGS})

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_LIST})
target_link_libraries(${PROJECT_NAME} CleoDecompiler)

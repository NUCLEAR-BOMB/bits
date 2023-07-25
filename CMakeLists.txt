cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project("bits" LANGUAGES CXX)

set(source "include/bits.hpp")

add_library("bits" INTERFACE ${source})
target_include_directories("bits" INTERFACE "include/")
target_compile_features("bits" INTERFACE "cxx_std_17")

target_compile_options("bits" INTERFACE $<$<CXX_COMPILER_ID:MSVC>:
	/Zc:__cplusplus /Zc:inline /Zc:preprocessor>
)

add_library(bits::bits ALIAS bits)

option(BITS_BUILD_TEST "Enable bits testing" ${PROJECT_IS_TOP_LEVEL})

if (BITS_BUILD_TEST)
	enable_testing()
	add_subdirectory(test)
endif()

function(add_clang_format_target name)
	find_program(clang_format_binary clang-format)
	if (clang_format_binary STREQUAL "clang_tidy_binary-NOTFOUND")
		message(NOTICE "[clang-format] clang-format not found")
		return()
	endif()
	file(GLOB_RECURSE format_files 
		"${PROJECT_SOURCE_DIR}/test/*.hpp" "${PROJECT_SOURCE_DIR}/test/*.cpp" "${PROJECT_SOURCE_DIR}/include/*.hpp"
	)
	add_custom_target(${name}
		COMMAND ${clang_format_binary} --verbose -i ${format_files}
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} VERBATIM
		SOURCES .clang-format
	)
endfunction()

add_clang_format_target(format)

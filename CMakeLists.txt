cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project("bits" LANGUAGES CXX)

option(BITS_CLANG_FORMAT "Adds the clang-format target 'format'" ON)
option(BITS_BUILD_TEST "Enable bits testing" ${PROJECT_IS_TOP_LEVEL})
option(BITS_BUILD_EXAMPLE "Enable bits example" ${PROJECT_IS_TOP_LEVEL})

set(source "include/bits.hpp")

add_library("bits" INTERFACE ${source})
target_include_directories("bits" INTERFACE "include/")
target_compile_features("bits" INTERFACE "cxx_std_17")

target_compile_options("bits" INTERFACE $<$<CXX_COMPILER_ID:MSVC>:
	/Zc:__cplusplus /Zc:inline /Zc:preprocessor>
)

add_library(bits::bits ALIAS bits)

if (BITS_BUILD_TEST)
	enable_testing()
	add_subdirectory(test)
endif()
if (BITS_BUILD_EXAMPLE)
    add_subdirectory(example)
endif()

function(add_clang_format_target name)
	find_program(clang_format_binary clang-format)
	if (clang_format_binary STREQUAL "clang_tidy_binary-NOTFOUND")
		message(NOTICE "[clang-format] clang-format not found")
		return()
	endif()

    set(format_files "")
    foreach(folder in ITEMS test include example)
        set(glob_folder "${PROJECT_SOURCE_DIR}/${folder}")
        file(GLOB_RECURSE folder_files "${glob_folder}/*.hpp" "${glob_folder}/*.cpp")
        list(APPEND format_files ${folder_files})
    endforeach()
	add_custom_target(${name}
		COMMAND ${clang_format_binary} --verbose -i ${format_files}
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} VERBATIM
		SOURCES .clang-format
	)
endfunction()

if (BITS_CLANG_FORMAT)
	add_clang_format_target(format)
endif()

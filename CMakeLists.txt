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

set(format_sources "")
foreach(folder IN ITEMS include test)
	file(GLOB_RECURSE folder_files "${folder}/*.hpp" "${folder}/*.cpp")
	list(APPEND format_sources ${folder_files})
endforeach()

add_custom_target(format
	COMMAND clang-format -i ${format_sources}
	DEPENDS ${format_sources}
	SOURCES .clang-format
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} VERBATIM)

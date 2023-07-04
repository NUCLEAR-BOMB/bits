cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project("bits" LANGUAGES CXX)

add_library("bits" INTERFACE "include/bits.hpp")
target_include_directories("bits" INTERFACE "include/")
target_compile_features("bits" INTERFACE "cxx_std_17")

target_compile_options("bits" INTERFACE $<$<CXX_COMPILER_ID:MSVC>:
	/Zc:__cplusplus /Zc:inline /Zc:preprocessor>
)

enable_testing()
add_subdirectory(test)

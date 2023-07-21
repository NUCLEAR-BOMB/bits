
option(BITS_GTEST_BRIEF "" NO)
option(BITS_GTEST_DEBUG_BREAK "" NO)

set(source basic.cpp operators.cpp test_types.hpp main.cpp)

add_executable("bits-test" ${source})
target_compile_options("bits-test" PRIVATE 
$<IF:$<CXX_COMPILER_ID:MSVC>,
	/Wall /WX /permissive- /Za /wd5264 /wd5262 /wd4625 /wd5026 /wd4626 /wd5027 /wd4820 /wd4514 /wd5246
	,
	-Wall -Wextra -Wpedantic -Wconversion -Werror
>)
target_include_directories("bits-test" PRIVATE .)

if (BITS_GTEST_BRIEF)
	list(APPEND bits_gtest_extra_args "\"--gtest_brief=1\"")
endif()
if (BITS_GTEST_DEBUG_BREAK)
	list(APPEND bits_gtest_extra_args "\"--gtest_break_on_failure\"" "\"--gtest_catch_exceptions=0\"")
endif()
if (bits_gtest_extra_args)
	string(REPLACE ";" "," bits_gtest_extra_args "${bits_gtest_extra_args}")
	target_compile_definitions("bits-test" PRIVATE EXTRA_ARGS=${bits_gtest_extra_args})
endif()

include(FetchContent)
FetchContent_Declare(
    googletest
	URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
	URL_HASH SHA256=ffa17fbc5953900994e2deec164bb8949879ea09b411e07f215bfbb1f87f4632
	FIND_PACKAGE_ARGS NAMES GTest
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK NO CACHE INTERNAL "")
set(INSTALL_GTEST NO CACHE INTERNAL "")

FetchContent_MakeAvailable(googletest)

set_target_properties("bits-test" PROPERTIES
	VS_GLOBAL_RunCodeAnalysis NO
	VS_GLOBAL_EnableMicrosoftCodeAnalysis NO
	VS_GLOBAL_EnableClangTidyCodeAnalysis YES
	CXX_CLANG_TIDY "clang-tidy;--use-color"
)

set_target_properties(gtest PROPERTIES VS_GLOBAL_EnableMicrosoftCodeAnalysis NO)
set_target_properties(gtest_main PROPERTIES VS_GLOBAL_EnableMicrosoftCodeAnalysis NO)

target_link_libraries("bits-test" PRIVATE bits::bits GTest::gtest)

#include(GoogleTest)
#gtest_discover_tests("bits-test")
#set_tests_properties(${list_of_tests} PROPERTIES ENVIRONMENT "GTEST_BRIEF=1;GTEST_BREAK_ON_FAILURE=1")

set(CMAKE_CTEST_ARGUMENTS "--output-on-failure")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_target_properties(gtest gtest_main PROPERTIES FOLDER "GoogleTest")

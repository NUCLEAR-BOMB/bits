
set(source basic.cpp operators.cpp)

add_executable("bits-test" ${source})
target_compile_options("bits-test" PRIVATE 
$<IF:$<CXX_COMPILER_ID:MSVC>,
	/Wall /WX /permissive- /Za /wd5264 /wd5262 /wd4625 /wd5026 /wd4626 /wd5027 /wd4820
	,
	-Wall -Wextra -Wpedantic -Wconversion -Werror
>)

set_target_properties("bits-test" PROPERTIES
    VS_GLOBAL_RunCodeAnalysis true
    VS_GLOBAL_EnableMicrosoftCodeAnalysis false
    VS_GLOBAL_EnableClangTidyCodeAnalysis true
	CXX_CLANG_TIDY clang-tidy
)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG v1.13.0
	FIND_PACKAGE_ARGS NAMES GTest
)
FetchContent_MakeAvailable(googletest)

target_link_libraries("bits-test" PRIVATE bits GTest::gtest GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests("bits-test")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_target_properties(gtest gtest_main gmock gmock_main PROPERTIES FOLDER "GoogleTest")

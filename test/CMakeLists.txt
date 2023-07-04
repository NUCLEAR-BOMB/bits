
set(source basic.cpp)

add_executable("bits-test" ${source})
target_compile_options("bits-test" PRIVATE 
$<IF:$<CXX_COMPILER_ID:MSVC>,
	/Wall /WX /permissive- /Za /wd5264 /wd5262
	,
	-Wall -Wextra -Wpedantic -Wconversion -Werror
>)

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

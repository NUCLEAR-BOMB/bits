
option(BITS_GTEST_BRIEF "Adds the --gtest_brief=1 argument for the test executable" NO)
option(BITS_GTEST_DEBUG_BREAK "Adds the --gtest_break_on_failure and --gtest_catch_exceptions=0 arguments for the test executable" NO)

option(BITS_CLANG_TIDY "Enables clang-tidy target 'tidy' for Makefile and Ninja generators" YES)

set(source basic.cpp operators.cpp test_types.hpp main.cpp slice.cpp)

add_executable("bits-test" ${source})
target_compile_options("bits-test" PRIVATE 
$<$<CXX_COMPILER_ID:MSVC>:
    /Wall /WX /permissive- /Za
    /wd5027 # 'type': move assignment operator was implicitly defined as deleted
    /wd4626 # 'derived class': assignment operator was implicitly defined as deleted because a base class assignment operator is inaccessible or deleted
    /wd5026 # 'type': move constructor was implicitly defined as deleted
    /wd4625 # 'derived class': copy constructor was implicitly defined as deleted because a base class copy constructor is inaccessible or deleted
    /wd5264 # 'variable-name': 'const' variable is not used
    /wd5262 # implicit fall-through occurs here between cases
    /wd4820 # 'bytes' bytes padding added after construct 'member_name'
    /wd4514 # 'function' : unreferenced inline function has been removed
    /wd5246 # 'member': the initialization of a subobject should be wrapped in braces
>
$<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
    -Wall # Enables all the warnings
    -Wextra # Enables some extra warning flags that are not enabled by -Wall
    -Wpedantic # Issue all the warnings demanded by strict ISO C and ISO C++
    -Wconversion # Warn for implicit conversions that may alter a value
    -Werror # Make all warnings into errors.
    -Wcast-align # Warn whenever a pointer is cast such that the required alignment of the target is increased
    -Wunused # -Wunused options combined
    -Wconversion # Warn for implicit conversions that may alter a value
    -Wsign-conversion # Warn for implicit conversions that may change the sign of an integer value
    -Wdouble-promotion # Give a warning when a value of type float is implicitly promoted to double
    -Wold-style-cast # Warn for C style casting
    -Wshadow # Warn whenever a local variable or type declaration shadows another variable
>)
target_include_directories("bits-test" PRIVATE .)

set_target_properties("bits-test" PROPERTIES
    VS_GLOBAL_RunCodeAnalysis NO
    VS_GLOBAL_EnableMicrosoftCodeAnalysis NO
    VS_GLOBAL_EnableClangTidyCodeAnalysis YES
    EXPORT_COMPILE_COMMANDS YES
    #CXX_CLANG_TIDY "clang-tidy;--use-color;--config-file=${PROJECT_SOURCE_DIR}/.clang-tidy;-p;${PROJECT_BINARY_DIR}/compile_commands.json"
)

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
set(BUILD_GMOCK NO CACHE BOOL "" FORCE)
set(INSTALL_GTEST NO CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

set_target_properties(gtest gtest_main PROPERTIES 
    VS_GLOBAL_EnableMicrosoftCodeAnalysis NO
    EXPORT_COMPILE_COMMANDS NO
)

#get_target_property(bits_gtest_include_dirs GTest::gtest INTERFACE_INCLUDE_DIRECTORIES)
#target_include_directories("bits-test" SYSTEM PRIVATE ${bits_gtest_include_dirs})

target_link_libraries("bits-test" PRIVATE bits::bits GTest::gtest)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_target_properties(gtest gtest_main PROPERTIES FOLDER "GoogleTest")

function(add_clang_tidy_target name)
    set(run_clang_tidy "${PROJECT_BINARY_DIR}/run-clang-tidy.py")

    file(DOWNLOAD https://raw.githubusercontent.com/llvm/llvm-project/main/clang-tools-extra/clang-tidy/tool/run-clang-tidy.py
        ${run_clang_tidy} STATUS download_status)

    list(GET download_status 0 status_code)
    if (NOT ${status_code} EQUAL 0)
        list(GET download_status 1 error_msg)
        message(NOTICE "[clang-tidy] Error occurred during download: ${error_msg}")
        return()
    endif()
    find_package(Python 3 QUIET)
    if (NOT Python_FOUND)
        message(NOTICE "[clang-tidy] Python3 not found")
        return()
    endif()
    find_program(clang_tidy_binary clang-tidy)
    if (clang_tidy_binary STREQUAL "clang_tidy_binary-NOTFOUND")
        message(NOTICE "[clang-tidy] clang-tidy not found")
        return()
    endif()

    set(clang_tidy_config "${PROJECT_SOURCE_DIR}/.clang-tidy")
    add_custom_target(${name}
        COMMAND ${Python_EXECUTABLE} "${run_clang_tidy}" -quiet -p "${PROJECT_BINARY_DIR}"
        $<$<NOT:$<BOOL:${MSVC_IDE}>>:-use-color>
        -clang-tidy-binary "${clang_tidy_binary}" -config-file "${clang_tidy_config}"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} VERBATIM
        DEPENDS ${run_clang_tidy}
        SOURCES ${clang_tidy_config}
    )
endfunction()

if (CMAKE_GENERATOR MATCHES "Unix Makefiles|Ninja" AND BITS_CLANG_TIDY)
    add_clang_tidy_target(tidy)
endif()

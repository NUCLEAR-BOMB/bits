
function(add_example name)
	set(sources ${ARGN})
	set(target_name "example-${name}")
	add_executable(${target_name} ${sources})
	target_link_libraries(${target_name} PRIVATE bits::bits)
	#set_target_properties(${target_name} PROPERTIES FOLDER "Bits examples")
endfunction()

#set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_example(hack-private hack_private.cpp)
add_example(copy-the-noncopyable copy_the_noncopyable.cpp)
add_example(float-to-int float_to_int.cpp)
add_example(hack-bool hack_bool.cpp)
add_example(modify-reference modify_reference.cpp)

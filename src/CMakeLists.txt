
set(target ant3d)
set(sources 
		main.cpp
	)

set ( LIBS
		necpp
	)

add_executable(${target} ${sources})
target_link_libraries(${target} ${LIBS})

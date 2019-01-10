
set(target ant3d)
set(sources 
		main.cpp
		scenegeometry.h
		scenegeometry.cpp
	)

set ( LIBS
		necpp
		OpenThreads
		osg
		osgViewer
		osgDB
	)

add_executable(${target} ${sources})
target_link_libraries(${target} ${LIBS})

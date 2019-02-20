
set(target ant3d)
set(sources 
		main.cpp
		scenegeometry.h
		scenegeometry.cpp
		hud.h
		hud.cpp
		utils.h
		utils.cpp
		necparser.h
		necparser.cpp
	)

set ( LIBS
		necpp
		OpenThreads
		osg
		osgViewer
		osgDB
		osgGA
		osgText
	)

add_executable(${target} ${sources})
target_link_libraries(${target} ${LIBS})


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
		simulation.h
		simulation.cpp
		antenna.h
		antenna.cpp
		ant_dipole.h
		ant_dipole.cpp
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

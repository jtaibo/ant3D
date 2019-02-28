#ifndef _ANTENNA_H_
#define _ANTENNA_H_

#include <nec2pp/c_geometry.h>
#include <osgGA/GUIEventHandler>

class Simulation;

class Antenna {
public:
	/// Antenna
	Antenna();

	virtual void configure(Simulation *sim);

	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter&);

protected:
	Simulation *_simulation;

	// TO-DO : connection to HUD

};

#endif // _ANTENNA_H_

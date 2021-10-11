#include "antenna.h"

Antenna::Antenna()
{
}

void Antenna::configure(Simulation *sim)
{
	_simulation = sim;
}

bool Antenna::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter&)
{
	// Must be redefined in subclasses
	return false;
}

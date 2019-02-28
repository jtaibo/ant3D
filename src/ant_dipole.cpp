#include "ant_dipole.h"
#include "simulation.h"

AntDipole::AntDipole() :
	// Default configuration
	_poleLength(4.68),
	_angle1(120.),
	_angle2(0.),
	_height(20.),
	_wireRadius(0.00069),	// 0.69mm radius (1.5mm2)
	_wireMaterial(0)
{
}

void AntDipole::configure(Simulation *sim)
{
	Antenna::configure(sim);
	c_geometry* geo = _simulation->getNECGeometry();

}


bool AntDipole::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	switch (ea.getEventType())
	{
	case(osgGA::GUIEventAdapter::KEYDOWN):
		{
			switch (ea.getKey()) {
			case 100:
				break;
			case 97:	// A -> switch antenna geometry visibility
				break;
			default:
				//			std::cout << "Key: " << ea.getKey() << std::endl;
				return false;
			}
			return true;
		}
		break;
	}
	return false;
}

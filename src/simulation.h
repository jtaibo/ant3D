#ifndef _SIMULATION_H_
#define _SIMULATION_H_

#include "antenna.h"

#include <nec2pp/c_geometry.h>
#include <nec2pp/libnecpp.h>
#include <nec2pp/nec_context.h>
#include <nec2pp/nec_radiation_pattern.h>

#include <osgGA/GUIEventHandler>


class Simulation {
public:
	/// Constructor
	Simulation();

	/// Destructor
	~Simulation();

	/// Configure the simulation from a NEC2 file
	void loadNEC2File(const std::string &cfg_file);

	/// Configure the simulation from an Antenna model
	void configure(Antenna *antenna);

	inline nec_radiation_pattern *getNECRadiationPattern() { return _radiationPattern; }
	inline c_geometry* getNECGeometry() { return _nec->get_geometry();  }

	/// Events received from SceneGeometry (GUIEventHandler00)
	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

protected:
	nec_context *_nec;
	nec_radiation_pattern* _radiationPattern;

	Antenna *_antenna;

	// Frequencies ?
	// Theta & phi divisions?
	// Ground ? <-- include with Antenna, here in Simulation or in a separate class?

	// TO-DO : connection to HUD
};

#endif //_SIMULATION_H_

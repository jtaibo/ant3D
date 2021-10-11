#ifndef _SIMULATION_H_
#define _SIMULATION_H_

#include "antenna.h"

#include <nec2pp/c_geometry.h>
#include <nec2pp/libnecpp.h>
#include <nec2pp/nec_context.h>
#include <nec2pp/nec_radiation_pattern.h>

#include <osgGA/GUIEventHandler>

class SceneVisualizer;

class Simulation {
public:
	/// Constructor
	Simulation();

	/// Destructor
	~Simulation();

	inline void setSceneVisualizer(SceneVisualizer *sv) { _sv = sv; }

	/// Configure the simulation from a NEC2 file
	void loadNEC2File(const std::string &cfg_file);

	/// Configure the simulation from an Antenna model
	void configure(Antenna *antenna);

	/// Configure the simulation (conserving the antenna model)
	void configure();

	inline nec_context *getNEC() { return _nec; }

	inline nec_radiation_pattern *getNECRadiationPattern() { return _radiationPattern; }
	inline c_geometry* getNECGeometry() { return _nec->get_geometry();  }
	inline Antenna *getAntenna() { return _antenna; }
	inline osg::Group *getAntennaGeometry() { return _antennaGeometry; }

	/// Events received from SceneGeometry (GUIEventHandler00)
	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	std::string getTextInfo();

protected:
	nec_context *_nec;
	nec_radiation_pattern* _radiationPattern;

	Antenna *_antenna;

	osg::ref_ptr<osg::Group> _antennaGeometry;

	SceneVisualizer *_sv;

	float _frequency;	// Target frequency (MHz)
	float _angDelta;	// Theta & phi divisions?
	bool _ground;		// Ground ? <-- include with Antenna, here in Simulation or in a separate class?


	// TO-DO : connection to HUD
};

#endif //_SIMULATION_H_

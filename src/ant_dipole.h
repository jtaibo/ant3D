#ifndef _ANT_DIPOLE_H_
#define _ANT_DIPOLE_H_

#include "antenna.h"

#include <osg/Geode>

/**
 *	Center fed balanced dipole
 *
 *	Adjustable
 */
class AntDipole : public Antenna
{
public:
	/// Constructor
	AntDipole();

	virtual void configure(Simulation *sim);

	/// Event management (received from SceneGeometry through Simulation class)
	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	virtual osg::Node *getScene();

	// to-do: Compute antenna geometry from target MHz, wire type/radius and available space
	// ...

private:
	float _poleLength;	///< Pole length (in meters)
	float _angle1;	///< V aperture (in degrees)
	float _angle2;	///< "Forward" angle (in degrees)
	float _height;	///< Height over ground (elev 0)  (in meters)
	float _wireRadius;	///< Wire radius (in meters)
	int _wireMaterial;	///< Material definition???

	/// Get textual information about the antenna (ready to be printed in the HUD)
	virtual std::string getTextInfo();
	// TO-DO: HUD connection

	void buildGeometry(osg::Vec3Array *vertices);
	osg::ref_ptr<osg::Geode> _root;

	void checkGeometry(osg::Vec3Array *vertices);

};

#endif //_ANT_DIPOLE_H_

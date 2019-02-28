#ifndef _SCENEGEOMETRY_H_
#define _SCENEGEOMETRY_H_

#include "simulation.h"

#include <osg/Node>
#include <osg/Switch>
#include <osgGA/GUIEventHandler>


/**
 *
 */
class SceneVisualizer : public osgGA::GUIEventHandler {

public:
	SceneVisualizer();

	void configure(Simulation *simulation);

	inline osg::Node *getScene() { return _root;  }

	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
	//	virtual void accept(osgGA::GUIEventHandlerVisitor& v);

private:

	/// Analize NEC2 computations and populate antenna information
	void analize(nec_radiation_pattern* rp);

	osg::Node *buildScene();
	osg::Node *buildAxes(float size=1.0);
	osg::Node *buildAntennaModel(c_geometry* geo);
	osg::Node *buildGroundGeometry();
	osg::Node *buildRadiationModel();

	/// Azimuthal (for any theta)
	osg::Node *buildAzimuthalGraph(int theta_idx=0);

	/// Elevation (for any phi)
	osg::Node *buildElevationGraph(int phi_idx=0);

	/// Histogram for the range
	osg::Node *buildHistogramGraph();

	/// Normalize gain 
	float normalizeGain(float dB);


private:
	Simulation *_simulation;

	osg::ref_ptr<osg::Group> _root;

	osg::ref_ptr<osg::Switch> _antenna;
	osg::ref_ptr<osg::Switch> _radiationPattern;

	int _nTheta;
	int _nPhi;

	float _gainRangeDB;	///< Gain range in dB
	float _minGainDB;	///< Min gain in dB
	float _maxGainDB;	///< Max gain in dB

	bool _showAntenna;
	bool _showRadiationPattern;
	bool _logScale;	///< Decibels (logarithmic) or power (linear)
};

#endif // _SCENEGEOMETRY_H_

#ifndef _SCENEGEOMETRY_H_
#define _SCENEGEOMETRY_H_

#include <osg/Node>
#include <osg/Switch>
#include <osgGA/GUIEventHandler>

#include <nec2pp/c_geometry.h>
#include <nec2pp/nec_radiation_pattern.h>

/**
 *
 */
class SceneVisualizer : public osgGA::GUIEventHandler {

public:
	SceneVisualizer();

	/// Analize NEC2 computations and populate antenna information
	void analize(nec_radiation_pattern* rp);

	osg::Node *buildScene(c_geometry* geo, nec_radiation_pattern* rp);

	osg::Node *buildAxes(float size=1.0);
	osg::Node *buildAntennaModel(c_geometry* geo);
	osg::Node *buildGroundGeometry(c_geometry* geo);
	osg::Node *buildRadiationModel();

	/// Azimuthal (for any theta)
	osg::Node *buildAzimuthalGraph(int theta_idx=0);

	/// Elevation (for any phi)
	osg::Node *buildElevationGraph(int phi_idx=0);

	/// Histogram for the range
	osg::Node *buildHistogramGraph();

	/// Normalize gain 
	float normalizeGain(float dB);

	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter&);
//	virtual void accept(osgGA::GUIEventHandlerVisitor& v);

private:
	c_geometry *_antennaGeometry;
	nec_radiation_pattern* _rp;

	osg::ref_ptr<osg::Group> _root;

	osg::ref_ptr<osg::Switch> _antenna;
	osg::ref_ptr<osg::Switch> _radiationPattern;

	int _nTheta;
	int _nPhi;

	float _gainRangeDB;	///< Gain range in dB
	float _minGainDB;	///< Min gain in dB
	float _maxGainDB;	///< Max gain in dB

	bool _logScale;	///< Decibels (logarithmic) or power (linear)
};

#endif // _SCENEGEOMETRY_H_

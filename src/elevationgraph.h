#ifndef _AZIMUTHALGRAPH_H_
#define _AZIMUTHALGRAPH_H_

#include "simulation.h"

#include <osg/Group>

class AzimuthalGraph {
public:

	/// Constructor
	AzimuthalGraph();

	inline osg::Node *getNode() { return _root;  }

	/// Plot the NEC2 simulation result
	void plotSimulationResult(Simulation &sim, int theta_idx);

private:

	void buildFrame();

	float _elevationAngle;	///< Elevation angle that the azimuthal graph corresponds to

	osg::ref_ptr<osg::Geode> _frame;
	osg::ref_ptr<osg::Geode> _plot;
	osg::ref_ptr<osg::Group> _root;
};

#endif // _AZIMUTHALGRAPH_H_

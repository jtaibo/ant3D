#ifndef _ELEVATIONGRAPH_H_
#define _ELEVATIONGRAPH_H_

#include "simulation.h"

#include <osg/Group>

class ElevationGraph {
public:

	/// Constructor
	ElevationGraph();

	inline osg::Node *getNode() { return _root;  }

	/// Plot the NEC2 simulation result
	void plotSimulationResult(Simulation &sim, int phi_idx, int theta_idx);

private:

	void buildFrame();

	float _azimuthAngle;	///< Azimuth angle that the elevation graph corresponds to

	osg::ref_ptr<osg::Geode> _frame;
	osg::ref_ptr<osg::Geode> _plot;
	osg::ref_ptr<osg::Group> _root;
};

#endif // _ELEVATIONGRAPH_H_

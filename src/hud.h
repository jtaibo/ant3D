#ifndef _HUD_H_
#define _HUD_H_

#include "simulation.h"

#include <osg/Group>

class HUD {
public:
	HUD(float min_dB, float max_dB, Simulation * sim);

	inline osg::Node *getNode() { return _root.get();  }

	osg::Node *buildDecibelScale(float min_dB, float max_dB, nec_radiation_pattern* rp);

	void refresh();

private:
	osg::ref_ptr<osg::Group> _root;

	Simulation *_simulation;
};

#endif // _HUD_H_

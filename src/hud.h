#ifndef _HUD_H_
#define _HUD_H_

#include <nec2pp/nec_radiation_pattern.h>

#include <osg/Group>

class HUD {
public:
	HUD(float min_dB, float max_dB, nec_radiation_pattern* rp);

	inline osg::Node *getNode() { return _root.get();  }

	osg::Node *buildDecibelScale(float min_dB, float max_dB, nec_radiation_pattern* rp);

private:
	osg::ref_ptr<osg::Group> _root;
};

#endif // _HUD_H_

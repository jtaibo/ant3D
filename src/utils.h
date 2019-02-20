#ifndef _UTILS_H_
#define _UTILS_H_

#include <osg/Vec4>

/// Convert value in range to pseudocolor
osg::Vec4 pseudoColor(float val, float min, float max);

/// Convert decibels to power value
inline float decibelsToPower(float dB)
{
	return pow(10., (dB / 10.));
}

/// Convert power value to decibels
inline float powerToDecibels(float P)
{
	return 10. * log10(P);
}

#endif // _UTILS_H_

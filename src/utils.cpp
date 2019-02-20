#include "utils.h"

#include <iostream>

/**
 * Convert value in range to pseudocolor
 */
osg::Vec4 pseudoColor(float val, float min, float max)
{

	float normalized_value = (val - min) / (max - min);

	float r, g, b;
	float a = 1.;

	float h = normalized_value;
	float s = 1.;
	float v = 1.;
	float i = floor(h * 6.);
	float f = h * 6. - i;
	float p = v * (1 - s);
	float q = v * (1 - f * s);
	float t = v * (1 - (1 - f) * s);

	switch ((int)i % 6) {
		case 0: 
			r = v, g = t, b = p; 
			break;
		case 1: 
			r = q, g = v, b = p; 
			break;
		case 2: 
			r = p, g = v, b = t; 
			break;
		case 3: 
			r = p, g = q, b = v; 
			break;
		case 4: 
			r = t, g = p, b = v; 
			break;
		case 5: 
			r = v, g = p, b = q; 
			break;
	}

	return (osg::Vec4(b, g, r, a));
}

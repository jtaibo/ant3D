#include "utils.h"

#define M_PI    3.14159265358979323846f   // pi
#define M_SQRT2	1.4142135623730950488016887242097f // sqrt(2.0)
#define M_C		299792458.0f	// speed of light (c) in m/s
#define M_VACUUM_PERMITTIVITY		8.854e-12 // Permittivity of vacuum

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


/**
 *	Compute skin depth or penetration depth in soil
 *
 *	ARRL Antenna Book, 3.1.2 (equation 1)
 *
 *	@param freq	Frequency (f) in Hz
 *	@param conductivity (sigma) in S/m siemens/meter
 *	@param permeability (mu) in henry/meter
 *	@param permittivity (epsilon) in farad/meter
 */
float skinDepth(float freq, float conductivity, float permeability, float permittivity)
{
	float omega = 2. * M_PI * freq;

	float delta = M_SQRT2 / (omega * sqrtf(permeability * permittivity))
		* pow( sqrtf(1 + powf(conductivity / (omega * permittivity), 2.)) - 1., -0.5 );

	return delta;
}

/**
 *	Wavelength in free space
 *
 *	ARRL Antenna Book, 3.1.3 (equation 2)
 *
 *	@param freq	Frequency (f) in Hz
 */
float waveLengthInFreeSpace(float freq)
{
	return M_C / freq;
}

/**
 *	Wavelength in soil
 *
 *	ARRL Antenna Book, 3.1.3 (equation 3)
 *
 *	@param freq	Frequency (f) in Hz
 *	@param relativePermittivity (epsilon sub r) (dimensionless)
 *	@param conductivity (sigma) in S/m siemens/meter
 */
inline float waveLengthInSoil(float freq, float relativePermittivity, float conductivity)
{
	float lambda_0 = waveLengthInFreeSpace(freq);
	float omega = 2. * M_PI * freq;
	float lambda = lambda_0 / powf(relativePermittivity*relativePermittivity + powf(conductivity / (omega * M_VACUUM_PERMITTIVITY), 2.), .25);
}

#include "scenegeometry.h"
#include "hud.h"
#include "utils.h"

#include <osgDB/ReadFile>


#define THETA_OFFSET -90.
#define THETA_FACTOR -1.
#define PHI_OFFSET 90.	// CHECK!!!!!
#define PHI_FACTOR 1.

/**
 *	Constructor
 */
SceneVisualizer::SceneVisualizer() :
	_showAntenna(true),
	_showRadiationPattern(true),
	_logScale(false)
{
	_root = new osg::Group();
}

void SceneVisualizer::configure(Simulation *simulation)
{
	_simulation = simulation;
	analize(simulation->getNECRadiationPattern());
	buildScene();
}


/**
 *	Analize NEC2 computations and populate antenna information
 */
void SceneVisualizer::analize(nec_radiation_pattern* rp)
{
	_nTheta = rp->get_ntheta();
	_nPhi = rp->get_nphi();

	// Compute dB range, minimum and maximum values
	_maxGainDB = -999.;
	_minGainDB = 999.;
	for (int theta_idx = 0; theta_idx < _nTheta; theta_idx++) {
		for (int phi_idx = 0; phi_idx < _nPhi; phi_idx++) {
			float power_gain_dB = rp->get_power_gain(theta_idx, phi_idx);
			if (power_gain_dB < _minGainDB && power_gain_dB >= -999.)
				_minGainDB = power_gain_dB;
			if (power_gain_dB > _maxGainDB)
				_maxGainDB = power_gain_dB;
			/*
						// dB to %
						float len = power;
			//			if (len == -999.)
			//				len = 0.;
			#endif
						float theta = rp->get_theta(theta_idx);
						float phi = rp->get_phi(phi_idx);
						theta = osg::DegreesToRadians(theta);
						phi = osg::DegreesToRadians(phi);
						osg::Vec3 pos = osg::Vec3(0., len, 0.)
								* osg::Matrix::rotate( theta, osg::X_AXIS)
								* osg::Matrix::rotate( phi, osg::Z_AXIS)
							;
						vertex_array->push_back(pos);
			*/
		}
	}
	_gainRangeDB = _maxGainDB - _minGainDB;
#if 0
	std::cout << "======================================================" << std::endl;
	std::cout << " min Gain (dB): " << _minGainDB << std::endl;
	std::cout << " max Gain (dB): " << _maxGainDB << std::endl;
	std::cout << " Gain range (dB): " << _gainRangeDB << std::endl;
	std::cout << "======================================================" << std::endl;
#endif
}

/**
 *
 */
osg::Node * SceneVisualizer::buildScene()
{
	_root->removeChildren(0, _root->getNumChildren());

//	_root->addChild( osgDB::readNodeFile("cow.osg") );
	_root->addChild(buildAxes(1.));
	_antenna = new osg::Switch();
	_root->addChild(_antenna);
	_antenna->addChild(buildAntennaModel(_simulation->getNECGeometry()), _showAntenna);
	_root->addChild(buildGroundGeometry());
	_radiationPattern = new osg::Switch();
	_root->addChild(_radiationPattern);
	_radiationPattern->addChild(buildRadiationModel(), _showRadiationPattern);
	// TO-DO: choose theta and phi values for azimuthal and elevation graphs
	_root->addChild(buildAzimuthalGraph(90));
	_root->addChild(buildElevationGraph(90));

	HUD *the_hud = new HUD(_minGainDB, _maxGainDB, _simulation->getNECRadiationPattern());
	_root->addChild(the_hud->getNode());

	return _root;
}

/**
 *
 */
osg::Node *SceneVisualizer::buildAxes(float size)
{
	osg::Geode *geode = new osg::Geode();
	osg::Geometry *geom = new osg::Geometry();

	// Vertex array
	osg::Vec3Array *vertex_array = new osg::Vec3Array();
	osg::Vec3 origin(0., 0., 0.);
	vertex_array->push_back(origin);
	vertex_array->push_back(osg::X_AXIS * size);
	vertex_array->push_back(origin);
	vertex_array->push_back(osg::Y_AXIS * size);
	vertex_array->push_back(origin);
	vertex_array->push_back(osg::Z_AXIS * size);
	geom->setVertexArray(vertex_array);

	// Normal array
	// Color array
	osg::Vec4Array *color_array = new osg::Vec4Array();
	color_array->push_back(osg::Vec4(1., 0., 0., 1.));	// Red
	color_array->push_back(osg::Vec4(1., 0., 0., 1.));
	color_array->push_back(osg::Vec4(0., 1., 0., 1.));	// Green
	color_array->push_back(osg::Vec4(0., 1., 0., 1.));
	color_array->push_back(osg::Vec4(0., 0., 1., 1.));	// Blue
	color_array->push_back(osg::Vec4(0., 0., 1., 1.));
	geom->setColorArray(color_array);
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	// Primitive sets
	osg::DrawArrays *pset = new osg::DrawArrays(GL_LINES, 0, vertex_array->size());

	geom->addPrimitiveSet(pset);

	geode->addChild(geom);
	return geode;
}

osg::Node *SceneVisualizer::buildGroundGeometry()
{
	osg::Geode *geode = new osg::Geode();
	osg::Geometry *geom = new osg::Geometry();

	// Vertex array
	osg::Vec3Array *vertex_array = new osg::Vec3Array();
	int n_sides = 10;
	float side_size = 10.;
	float incr = side_size / (float)n_sides;

	osg::Vec3 pos(-side_size / 2., -side_size / 2., 0.);
	for (int i = 0; i <= n_sides; i++) {
		pos.y() = -side_size / 2.;
		vertex_array->push_back(pos);
		pos.y() += side_size;
		vertex_array->push_back(pos);
		pos.x() += incr;
	}
	pos.y() = -side_size / 2.;
	for (int i = 0; i <= n_sides; i++) {
		pos.x() = -side_size / 2.;
		vertex_array->push_back(pos);
		pos.x() += side_size;
		vertex_array->push_back(pos);
		pos.y() += incr;
	}

	geom->setVertexArray(vertex_array);

	// Normal array
	// Color array
	osg::Vec4Array *color_array = new osg::Vec4Array();
	color_array->push_back(osg::Vec4(.5, .5, 0., 1.));
	geom->setColorArray(color_array);
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);

	// Primitive sets
	osg::DrawArrays *pset = new osg::DrawArrays(GL_LINES, 0, vertex_array->size());

	geom->addPrimitiveSet(pset);

	geode->addChild(geom);
	return geode;
}

/**
 *
 */
osg::Node *SceneVisualizer::buildAntennaModel(c_geometry* geo)
{
	osg::Geode *geode = new osg::Geode();
	osg::Geometry *geom = new osg::Geometry();

	std::cout << "Antenna definition" << std::endl;
	std::cout << geo->n_segments << " segments" << std::endl;

	// Vertex array
	osg::Vec3Array *vertex_array = new osg::Vec3Array();
	// for each wire
	for (int i = 0; i < geo->n_segments; i++) {
//		vertex_array->push_back(osg::Vec3(geo->x[i], geo->y[i], geo->z[i]));
		vertex_array->push_back(osg::Vec3(geo->x2[i], geo->y2[i], geo->z2[i]));
	}
	geom->setVertexArray(vertex_array);

	// Normal array
	// Color array
	osg::Vec4Array *color_array = new osg::Vec4Array();
	color_array->push_back(osg::Vec4(1., 1., 0., 1.));
	geom->setColorArray(color_array);
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);

	// Primitive sets
	osg::DrawArrays *pset = new osg::DrawArrays(GL_LINE_STRIP, 0, vertex_array->size());
	geom->addPrimitiveSet(pset);
/*
	pset = new osg::DrawArrays(GL_POINTS, 0, vertex_array->size());
	geom->addPrimitiveSet(pset);
*/
	geode->addChild(geom);
	return geode;
}

/**
 *
 */
osg::Node *SceneVisualizer::buildRadiationModel()
{
	osg::Geode *geode = new osg::Geode();
	osg::Geometry *geom = new osg::Geometry();

	// Vertex array
	osg::Vec3Array *vertex_array = new osg::Vec3Array();

	for (int theta_idx = 0; theta_idx < _nTheta; theta_idx++) {
		for (int phi_idx = 0; phi_idx < _nPhi; phi_idx++) {

			float power_gain_dB = _simulation->getNECRadiationPattern()->get_power_gain(theta_idx, phi_idx);
			float normalized_gain = normalizeGain(power_gain_dB);

			float theta = _simulation->getNECRadiationPattern()->get_theta(theta_idx);
			float phi = _simulation->getNECRadiationPattern()->get_phi(phi_idx);
			theta = osg::DegreesToRadians(theta + THETA_OFFSET) * THETA_FACTOR;
			phi = osg::DegreesToRadians(phi + PHI_OFFSET) * PHI_FACTOR;
			osg::Vec3 pos = osg::Vec3(0., normalized_gain, 0.)
				* osg::Matrix::rotate(theta, osg::X_AXIS)
				* osg::Matrix::rotate(phi, osg::Z_AXIS)
				;
			vertex_array->push_back(pos);
		}
	}

	geom->setVertexArray(vertex_array);

	// Normal array

	// Color array
	osg::Vec4Array *color_array = new osg::Vec4Array();
	geom->setColorArray(color_array);
	for (int theta_idx = 0; theta_idx < _nTheta; theta_idx++) {
		for (int phi_idx = 0; phi_idx < _nPhi; phi_idx++) {
			float gain_dB = _simulation->getNECRadiationPattern()->get_power_gain(theta_idx, phi_idx);
#if 1
			color_array->push_back(pseudoColor(gain_dB, _minGainDB, _maxGainDB));
#else
			float normalized_gain = normalizeGain(gain_dB);
			color_array->push_back( pseudoColor(normalized_gain, _minGainDB, _maxGainDB) );
#endif
		}
	}
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	// Primitive sets
	osg::UIntArray *idx_array = new osg::UIntArray();
	for (int theta_idx = 0; theta_idx < _nTheta-1; theta_idx++) {
		for (int phi_idx = 0; phi_idx < _nPhi; phi_idx++) {
			unsigned int idx = theta_idx * _nPhi + phi_idx;
			idx_array->push_back(idx);
			if (phi_idx == _nPhi - 1) {
				// last column
				idx_array->push_back(idx + 1 - _nPhi);	// wrap around the "toroidal" structure
				idx_array->push_back(idx + 1);	// wrap around the "toroidal" structure
			}
			else {
				idx_array->push_back(idx + 1);
				idx_array->push_back(idx + 1 + _nPhi);
			}
			idx_array->push_back(idx + _nTheta);
		}
	}
	osg::DrawElements *pset = new osg::DrawElementsUInt(GL_QUADS, idx_array->size(), (const GLuint *)idx_array->getDataPointer());
	geom->addPrimitiveSet(pset);

	geode->addChild(geom);
	return geode;
}

/**
 *
 */
 osg::Node *SceneVisualizer::buildAzimuthalGraph(int theta_idx)
{
	osg::Geode *geode = new osg::Geode();
	osg::Geometry *geom = new osg::Geometry();

	// Vertex array
	osg::Vec3Array *vertex_array = new osg::Vec3Array();
	for (int phi_idx = 0; phi_idx < _nPhi; phi_idx++) {

		float power_gain_dB = _simulation->getNECRadiationPattern()->get_power_gain(theta_idx, phi_idx);
		power_gain_dB = osg::maximum(power_gain_dB, _minGainDB);
		float normalized_gain = normalizeGain(power_gain_dB);

		float theta = _simulation->getNECRadiationPattern()->get_theta(theta_idx);
		float phi = _simulation->getNECRadiationPattern()->get_phi(phi_idx);
		theta = osg::DegreesToRadians(theta + THETA_OFFSET) * THETA_FACTOR;
		phi = osg::DegreesToRadians(phi + PHI_OFFSET) * PHI_FACTOR;
		osg::Vec3 pos = osg::Vec3(0., normalized_gain, 0.)
			* osg::Matrix::rotate(theta, osg::X_AXIS)
			* osg::Matrix::rotate(phi, osg::Z_AXIS)
			;
		vertex_array->push_back(pos);
	}
	geom->setVertexArray(vertex_array);

	// Normal array

	// Color array
	osg::Vec4Array *color_array = new osg::Vec4Array();
	geom->setColorArray(color_array);
	color_array->push_back(osg::Vec4(0., 1., 1., 1.));
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);

	// Primitive sets
	osg::DrawArrays *pset = new osg::DrawArrays(GL_LINE_LOOP, 0, vertex_array->size());
	geom->addPrimitiveSet(pset);

	geode->addChild(geom);
	return geode;
}

/**
 *
 */
 osg::Node *SceneVisualizer::buildElevationGraph(int phi_idx)
{
	osg::Geode *geode = new osg::Geode();
	osg::Geometry *geom = new osg::Geometry();

	// To print elevation graph in 360º we use two opposite phi angles to build the two halves of the curve

	// Vertex array
	osg::Vec3Array *vertex_array = new osg::Vec3Array();

	// First half
	for (int theta_idx = 0; theta_idx < _nTheta; theta_idx++) {

		float power_gain_dB = _simulation->getNECRadiationPattern()->get_power_gain(theta_idx, phi_idx);
		float normalized_gain = normalizeGain(power_gain_dB);

		float theta = _simulation->getNECRadiationPattern()->get_theta(theta_idx);
		float phi = _simulation->getNECRadiationPattern()->get_phi(phi_idx);
		theta = osg::DegreesToRadians(theta + THETA_OFFSET) * THETA_FACTOR;
		phi = osg::DegreesToRadians(phi + PHI_OFFSET) * PHI_FACTOR;
		osg::Vec3 pos = osg::Vec3(0., normalized_gain, 0.)
			* osg::Matrix::rotate(theta, osg::X_AXIS)
			* osg::Matrix::rotate(phi, osg::Z_AXIS)
			;
		vertex_array->push_back(pos);
	}
	// Second half
	phi_idx = phi_idx + (_nPhi / 2) % _nPhi;
	for (int theta_idx = _nTheta-1; theta_idx >= 0; theta_idx--) {

		float power_gain_dB = _simulation->getNECRadiationPattern()->get_power_gain(theta_idx, phi_idx);
		float normalized_gain = normalizeGain(power_gain_dB);

		float theta = _simulation->getNECRadiationPattern()->get_theta(theta_idx);
		float phi = _simulation->getNECRadiationPattern()->get_phi(phi_idx);
		theta = osg::DegreesToRadians(theta + THETA_OFFSET) * THETA_FACTOR;
		phi = osg::DegreesToRadians(phi + PHI_OFFSET) * PHI_FACTOR;
		osg::Vec3 pos = osg::Vec3(0., normalized_gain, 0.)
			* osg::Matrix::rotate(theta, osg::X_AXIS)
			* osg::Matrix::rotate(phi, osg::Z_AXIS)
			;
		vertex_array->push_back(pos);
	}

	geom->setVertexArray(vertex_array);

	// Normal array

	// Color array
	osg::Vec4Array *color_array = new osg::Vec4Array();
	geom->setColorArray(color_array);
	color_array->push_back(osg::Vec4(1., 0., 1., 1.));
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);

	// Primitive sets
	osg::DrawArrays *pset = new osg::DrawArrays(GL_LINE_LOOP, 0, vertex_array->size());
	geom->addPrimitiveSet(pset);

	geode->addChild(geom);
	return geode;
}

/**
 *	Histogram for the range
 */
osg::Node *buildHistogramGraph(nec_radiation_pattern* rp)
{
	// TO-DO: Implement me!
	return NULL;
}

/**
 *	Normalize gain 
 */
float SceneVisualizer::normalizeGain(float dB)
{
	if (_logScale) {
		return (dB - _minGainDB) / _gainRangeDB;
	}
	else {
		return decibelsToPower(dB);
	}
}


/**
 *
 */
bool SceneVisualizer::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	bool processed = false;
	switch (ea.getEventType())
	{
	case(osgGA::GUIEventAdapter::KEYDOWN):
		{
			processed = true;
			switch (ea.getKey()) {
			case 100:	// D -> switch linear/logarithmic scale
				_logScale = !_logScale;
				std::cout << "Setting log scale to " << _logScale << std::endl;
				buildScene();
				break;
			case 97:	// A -> switch antenna geometry visibility
				_showAntenna = !_showAntenna;
				_antenna->setValue(0, _showAntenna);
				break;
			case 114:	// R -> switch radiation pattern visibility
				_showRadiationPattern = !_showRadiationPattern;
				_radiationPattern->setValue(0, _showRadiationPattern);
				break;
			default:
				std::cout << "Key: " << ea.getKey() << std::endl;
				processed = false;
			}
		}
		break;
	default:
		break;
	}
	if (!processed) {
		processed = _simulation->handle(ea, aa);
	}
	return processed;
}

/**
 *
 */
/*void SceneVisualizer::accept(osgGA::GUIEventHandlerVisitor& v)
{
	v.visit(*this);
}
*/
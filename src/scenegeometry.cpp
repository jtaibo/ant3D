#include "scenegeometry.h"

#include <osgDB/ReadFile>


osg::Node * buildScene(c_geometry* geo, nec_radiation_pattern* rp)
{
	osg::Group *group = new osg::Group();

//	group->addChild( osgDB::readNodeFile("cow.osg") );
//	group->addChild(buildAntennaModel(geo));
	group->addChild(buildRadiationModel(rp));

	return group;
}

osg::Node *buildAntennaModel(c_geometry* geo)
{
	osg::Geode *geode = new osg::Geode();
	osg::Geometry *geom = new osg::Geometry();

	// Vertex array
	osg::Vec3Array *vertex_array = new osg::Vec3Array();
	// for each wire
#if 0
	for (geo->) {
		// for each segment
		for () {

		}
	}
#else
	vertex_array->push_back(osg::Vec3(0., 0., 0.));
	vertex_array->push_back(osg::Vec3(1., 0., 0.));
	vertex_array->push_back(osg::Vec3(1., 1., 0.));
	vertex_array->push_back(osg::Vec3(0., 1., 0.));
	vertex_array->push_back(osg::Vec3(0., 1., 1.));
	vertex_array->push_back(osg::Vec3(1., 1., 1.));
	vertex_array->push_back(osg::Vec3(1., 0., 1.));
	vertex_array->push_back(osg::Vec3(0., 0., 1.));
#endif
	geom->setVertexArray(vertex_array);

	// Normal array
	// Color array
	osg::Vec4Array *color_array = new osg::Vec4Array();
	color_array->push_back(osg::Vec4(1., 1., 0., 1.));
	geom->setColorArray(color_array);
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);

	// Primitive sets
	osg::DrawArrays *pset = new osg::DrawArrays(GL_LINE_STRIP, 0, 8);

	geom->addPrimitiveSet(pset);

	geode->addChild(geom);
	return geode;
}

/**
 *
 */
osg::Node *buildRadiationModel(nec_radiation_pattern* rp)
{
	osg::Geode *geode = new osg::Geode();
	osg::Geometry *geom = new osg::Geometry();

	// Vertex array
	osg::Vec3Array *vertex_array = new osg::Vec3Array();

	int ntheta = rp->get_ntheta();
	int nphi = rp->get_nphi();

	std::cout << " thetas : " << ntheta
		<< " start: " << rp->get_theta_start() << " delta: " << rp->get_delta_theta()
		<< " phis: " << rp->get_nphi()
		<< " start: " << rp->get_phi_start() << " delta: " << rp->get_delta_phi()
		<< std::endl;
	for (int i = 0; i < ntheta; i++) {
		for (int j = 0; j < nphi; j++) {
#if 0
			float len = 1.;
#else
			float len = rp->get_power_gain(i, j);
#endif
			float theta = rp->get_theta(i);
			float phi = rp->get_phi(j);
			theta = osg::DegreesToRadians(theta);
			phi = osg::DegreesToRadians(phi);
			osg::Vec3 pos = osg::Vec3(0., len, 0.)
					* osg::Matrix::rotate( theta, osg::X_AXIS)
					* osg::Matrix::rotate( phi, osg::Z_AXIS)
				;
			vertex_array->push_back(pos);
		}
	}

	geom->setVertexArray(vertex_array);

	// Normal array
	// Color array
	osg::Vec4Array *color_array = new osg::Vec4Array();
	geom->setColorArray(color_array);
#if 0 // monocolor
	color_array->push_back(osg::Vec4(1., 1., 0., 1.));
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);
#else
	for (int i = 0; i < ntheta - 1; i++) {
		for (int j = 0; j < nphi - 1; j++) {
			float val = rp->get_power_gain(i, j);
			// TO-DO: convert val to color
			color_array->push_back(osg::Vec4(val, val, val, 1.));
		}
	}
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
#endif

	// Primitive sets
	osg::UIntArray *idx_array = new osg::UIntArray();
	for (int i = 0; i < ntheta-1; i++) {
		for (int j = 0; j < nphi-1; j++) {
			unsigned int idx = j * ntheta + i;
			idx_array->push_back(idx);
			idx_array->push_back(idx+1);
			idx_array->push_back(idx+1 + ntheta);
			idx_array->push_back(idx + ntheta);
		}
	}
	osg::DrawElements *pset = new osg::DrawElementsUInt(GL_QUADS, idx_array->size(), (const GLuint *)idx_array->getDataPointer());
	geom->addPrimitiveSet(pset);

	geode->addChild(geom);
	return geode;
}

#include "scenegeometry.h"

#include <osgDB/ReadFile>


osg::Node * buildScene(c_geometry* geo, nec_radiation_pattern* rp)
{
	osg::Group *group = new osg::Group();

	group->addChild( osgDB::readNodeFile("cow.osg") );
	group->addChild(buildAntennaModel(geo));
	group->addChild(buildRadiationModel(rp));

	return group;
}


osg::Node *buildAntennaModel(c_geometry* geo)
{
	return NULL;
}


osg::Node *buildRadiationModel(nec_radiation_pattern* rp)
{
	return NULL;
}

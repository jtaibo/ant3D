#ifndef _SCENEGEOMETRY_H_
#define _SCENEGEOMETRY_H_

#include <osg/Node>

#include <nec2pp/c_geometry.h>
#include <nec2pp/nec_radiation_pattern.h>

osg::Node *buildScene(c_geometry* geo, nec_radiation_pattern* rp);

osg::Node *buildAntennaModel(c_geometry* geo);
osg::Node *buildRadiationModel(nec_radiation_pattern* rp);

#endif // _SCENEGEOMETRY_H_

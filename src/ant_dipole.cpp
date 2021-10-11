#include "ant_dipole.h"
#include "simulation.h"

#include <osg/Geode>
#include <osg/Geometry>


AntDipole::AntDipole() :
	// Default configuration
	_poleLength(5.165),
	_angle1(90.),
	_angle2(0.),
	_height(5.),
	_wireRadius(0.00069),	// 0.69mm radius (1.5mm2)
	_wireMaterial(0)
{
}

void AntDipole::checkGeometry(osg::Vec3Array *vertices)
{
	float min_height = 10e9;

	for (int i = 0; i < vertices->size(); i++) {
		osg::Vec3 &v = (*vertices)[i];
		if (v.z() < min_height )
			min_height = v.z();
	}

	if (min_height < 0.) {
		for (int i = 0; i < vertices->size(); i++) {
			(*vertices)[i].z() -= min_height;
		}
		_height -= min_height;
	}
}

#if 0
void AntDipole::configure(Simulation *sim)
{
	Antenna::configure(sim);
	c_geometry* geo = _simulation->getNECGeometry();

	// Place a short horizontal wire in the center, that connect to both poles of the dipole
	// This short wire will be fed at its center
	float center_wire_width = .05;	// ***to-do: compute size   (5cm right now)

	/*
	   Origin
		  |
		  v
	  p1 _._ p2
		/   \
	   /     \
	  /       \
	 /         \
	/           \
   p0           p3

   The dipole is place along the Y axis
   The coordinate origin is initially placed in the center (elevated with _height attribute)

	*/

	osg::Vec3 p0(-_poleLength, 0., 0.);
	osg::Vec3 p1(-center_wire_width / 2., 0., 0.);
	osg::Vec3 p2(center_wire_width / 2., 0., 0.);
	osg::Vec3 p3(_poleLength, 0., 0.);

	p0 = p0
		* osg::Matrix::translate(center_wire_width / 2., 0., 0.)
		* osg::Matrix::rotate(-osg::DegreesToRadians(_angle1 / 2.), osg::Y_AXIS)
		* osg::Matrix::rotate(osg::DegreesToRadians(_angle2), osg::X_AXIS)
		* osg::Matrix::translate(-center_wire_width / 2., 0., 0.);

	p3 = p3
		* osg::Matrix::translate(-center_wire_width / 2., 0., 0.)
		* osg::Matrix::rotate(osg::DegreesToRadians(_angle1 / 2.), osg::Y_AXIS)
		* osg::Matrix::rotate(osg::DegreesToRadians(_angle2), osg::X_AXIS)
		* osg::Matrix::translate(center_wire_width / 2., 0., 0.);

	// Elevate _height meters over coordinate origin (ground)
	p0.z() += _height;
	p1.z() += _height;
	p2.z() += _height;
	p3.z() += _height;

	// Build geometry
	osg::Vec3Array *vertices = new osg::Vec3Array();
	{
		vertices->push_back(p0);
		vertices->push_back(p1);
		vertices->push_back(p2);
		vertices->push_back(p3);
	}
	buildGeometry(vertices);


	osg::Vec3 w1, w2;
	int segment_count = 25;	// should be around 12-20 per wavelength
	float rdel = 1.;	// uniform length
	float rrad = 1.;	// not tapered

	w1 = (*vertices)[0];
	w2 = (*vertices)[1];
	geo->wire(1, segment_count, w1.x(), w1.y(), w1.z(), w2.x(), w2.y(), w2.z(), _wireRadius, rdel, rrad);
	w1 = (*vertices)[1];
	w2 = (*vertices)[2];
	geo->wire(2, segment_count, w1.x(), w1.y(), w1.z(), w2.x(), w2.y(), w2.z(), _wireRadius, rdel, rrad);
	w1 = (*vertices)[2];
	w2 = (*vertices)[3];
	geo->wire(3, segment_count, w1.x(), w1.y(), w1.z(), w2.x(), w2.y(), w2.z(), _wireRadius, rdel, rrad);

	// Geometry complete!
	_simulation->getNEC()->geometry_complete(0);

	// No ground
	_simulation->getNEC()->gn_card(-1, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

	// Excitation in the center wire (tag 2) (WARNING: beware of segment count!!! --> 25 segments -- middle in segment 13)
	_simulation->getNEC()->ex_card(EXCITATION_LINEAR, 2, 13, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

}
#endif

void AntDipole::configure(Simulation *sim)
{
	Antenna::configure(sim);
	c_geometry* geo = _simulation->getNECGeometry();

	// Place a short horizontal wire in the center, that connect to both poles of the dipole
	// This short wire will be fed at its center
	float center_wire_width = .05;	// ***to-do: compute size   (5cm right now)

	/*
	   Origin
	      |
	      v
	  p1 _._ p2
	    /   \
	   /     \
	  /       \
	 /         \
	/           \
   p0           p3

   The dipole is place along the Y axis
   The coordinate origin is initially placed in the center (elevated with _height attribute)

	*/

	osg::Vec3 p0( -_poleLength, 0. , 0. );
	osg::Vec3 p1( 0., 0., 0. );
	osg::Vec3 p2( _poleLength, 0., 0. );

	p0 = p0
		* osg::Matrix::rotate(-osg::DegreesToRadians(_angle1 / 2.), osg::Y_AXIS)
		* osg::Matrix::rotate(osg::DegreesToRadians(_angle2), osg::X_AXIS);

	p2 = p2
		* osg::Matrix::rotate(osg::DegreesToRadians(_angle1 / 2.), osg::Y_AXIS)
		* osg::Matrix::rotate(osg::DegreesToRadians(_angle2), osg::X_AXIS);

	// Elevate _height meters over coordinate origin (ground)
	p0.z() += _height;
	p1.z() += _height;
	p2.z() += _height;

	// Build geometry
	osg::Vec3Array *vertices = new osg::Vec3Array();
	{
		vertices->push_back(p0);
		vertices->push_back(p1);
		vertices->push_back(p2);
	}
	buildGeometry(vertices);


	osg::Vec3 w1, w2;
	int segment_count = 25;	// should be around 12-20 per wavelength
	float rdel = 1.;	// uniform length
	float rrad = 1.;	// not tapered

	w1 = (*vertices)[0];
	w2 = (*vertices)[1];
	geo->wire(1, segment_count, w1.x(), w1.y(), w1.z(), w2.x(), w2.y(), w2.z(), _wireRadius, rdel, rrad);
	w1 = (*vertices)[1];
	w2 = (*vertices)[2];
	geo->wire(2, segment_count, w1.x(), w1.y(), w1.z(), w2.x(), w2.y(), w2.z(), _wireRadius, rdel, rrad);

#if 0 // No ground
	// Geometry complete! (no ground)
	_simulation->getNEC()->geometry_complete(0);
	_simulation->getNEC()->gn_card(-1, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
#else
	// Ground
	_simulation->getNEC()->geometry_complete(1);
	_simulation->getNEC()->gn_card(1, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
#endif

	// Excitation in the center wire (tag 2) (WARNING: beware of segment count!!! --> 25 segments -- middle in segment 13)
	_simulation->getNEC()->ex_card(EXCITATION_LINEAR, 2, 0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

}


bool AntDipole::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	switch (ea.getEventType())
	{
	case(osgGA::GUIEventAdapter::KEYDOWN):
		{
			switch (ea.getKey()) {
			case 104:	// H
				_height += 1.0;
				_simulation->configure();
				break;
			case 110:	// N
				_height -= 1.0;
				_simulation->configure();
				break;
			case 106:	// J
				_angle1 += 1.0;
				_simulation->configure();
				break;
			case 109:	// M
				_angle1 -= 1.0;
				_simulation->configure();
				break;
			case 97:	// A -> switch antenna geometry visibility
				break;
			default:
				//			std::cout << "Key: " << ea.getKey() << std::endl;
				return false;
			}
			return true;
		}
		break;
	}
	return false;
}

osg::Node *AntDipole::getScene()
{
	return _root.get();
}


void AntDipole::buildGeometry(osg::Vec3Array *vertices)
{
	osg::Geode *geode = new osg::Geode();
	osg::Geometry *geom = new osg::Geometry();

	// Check integrity of vertex array
	checkGeometry(vertices);

	// Vertex array is built in configure() method and passed as an argument to buildGeometry()
	geom->setVertexArray(vertices);

	// Normal array
	// Color array
	osg::Vec4Array *color_array = new osg::Vec4Array();
	color_array->push_back(osg::Vec4(1., 1., 0., 1.));
	geom->setColorArray(color_array);
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);

	// Primitive sets
	osg::DrawArrays *pset = new osg::DrawArrays(GL_LINE_STRIP, 0, vertices->size());
	geom->addPrimitiveSet(pset);
	geode->addChild(geom);

	_root = geode;
}

std::string AntDipole::getTextInfo()
{
	std::stringstream info;
	info << "Height: " << _height << " m" << std::endl;
	info << "Angle: " << _angle1 << "º, " << _angle2 << "º" << std::endl;
	return info.str();
}

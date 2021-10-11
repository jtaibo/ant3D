#include "simulation.h"
#include "necparser.h"
#include "scenegeometry.h"

/**
 *	Constructor
 */
Simulation::Simulation() :
	_nec(NULL),
	_radiationPattern(NULL),
	_antenna(NULL),
	_sv(NULL)
{
	_antennaGeometry = new osg::Group();
}

/**
 *	Destructor
 */
Simulation::~Simulation()
{
	if ( _nec )
		nec_delete(_nec);
}

/**
 *	Load NEC2 file configuration
 */
void Simulation::loadNEC2File(const std::string &cfg_file)
{
	if (_antenna) {
		delete _antenna;
		_antenna = NULL;
	}
	if (_nec)
		nec_delete(_nec);

	_nec = create_nec_context_from_file(cfg_file);
	if (_nec) {
		_radiationPattern = _nec->get_radiation_pattern(0);
	}
	else {
		std::cerr << "ERROR reading NEC2 file" << std::endl;
	}
}

/**
 *	Configure the simulation from an Antenna model
 */
void Simulation::configure(Antenna *antenna)
{
	if (_antenna)
		delete _antenna;
	_antenna = antenna;

	configure();
}

/**
 *	Configure the simulation (with the same antenna model)
 */
void Simulation::configure()
{
	if (_nec)
		nec_delete(_nec);

	_nec = nec_create();
	_nec->initialize();
	_antenna->configure(this);

	_antennaGeometry->removeChildren(0, _antennaGeometry->getNumChildren());
	_antennaGeometry->addChild(_antenna->getScene());

	_nec->fr_card(0, 1, 14.200, 0.0);

	float delta = 5.;
#if 1 // HEMISPHERE
	_nec->rp_card(0, 90 / delta, 360 / delta, 0, 5, 0, 0, 0.0, 0.0, delta, delta, 0.0, 0.0);
#else // SPHERE
	_nec->rp_card(0, 180 / delta, 360 / delta, 0, 5, 0, 0, 0.0, 0.0, delta, delta, 0.0, 0.0);
#endif

	_radiationPattern = _nec->get_radiation_pattern(0);

	if (_sv)
		_sv->configure(this);
}

bool Simulation::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	bool processed = false;
	switch (ea.getEventType())
		{
		case(osgGA::GUIEventAdapter::KEYDOWN):
		{
			processed = true;
			switch (ea.getKey()) {
			case 100:
				break;
			default:
	//			std::cout << "Key: " << ea.getKey() << std::endl;
				processed = false;
			}
		}
		break;
	default:
		break;
	}
	if (!processed && _antenna) {
		processed = _antenna->handle(ea, aa);
	}
	return processed;
}


std::string Simulation::getTextInfo()
{
	std::stringstream txtstr;
	txtstr << "Frequency: " << _frequency << " MHz" << std::endl;
//	txtstr << "Lambda " << 
	if ( _antenna )
		txtstr << _antenna->getTextInfo().c_str() << std::endl;
	return txtstr.str();
}

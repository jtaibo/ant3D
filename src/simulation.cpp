#include "simulation.h"
#include "necparser.h"

/**
 *	Constructor
 */
Simulation::Simulation() :
	_nec(NULL),
	_radiationPattern(NULL),
	_antenna(NULL)
{
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
	if (_nec)
		nec_delete(_nec);

	_antenna = antenna;
	_nec->initialize();
	_antenna->configure(this);
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

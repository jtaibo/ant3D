#include "scenegeometry.h"
#include "simulation.h"
#include "ant_dipole.h"

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/StateSetManipulator>


#define BACKGROUND_COLOR osg::Vec4(.5, .5, .5, 1.)


void printNECInfo(nec_radiation_pattern *rp)
{
	int nth = rp->get_ntheta();
	int nph = rp->get_nphi();

	cout << endl << "Theta \tPhi \tHorizontal \tVertical \tTotal" << endl;
	for (int j = 0; j < nph; j++) {
		for (int i = 0; i < nth; i++) {
			cout
				<< rp->get_theta(i) << "  \t"
				<< rp->get_phi(j) << "  \t"
				<< rp->get_power_gain_horiz(i, j) << "  \t"
				<< rp->get_power_gain_vert(i, j) << "  \t"
				<< rp->get_power_gain(i, j) << "  \t"
				<< rp->get_etheta_magnitude(i, j) << "  \t"
				<< rp->get_etheta_phase(i, j) << "  \t"
				<< rp->get_ephi_magnitude(i, j) << "  \t"
				<< rp->get_ephi_phase(i, j)
				<< endl;
		}
	}
}


void syntax(char *program_name)
{
	std::cout << "Syntax:" << std::endl;
	std::cout << "\t" << program_name << " <nec_input_file>" << std::endl;
	// TO-DO: options to build antenna presets from some tunable parameters
}

int main(int argc, char *argv[])
{
	std::cout << "Antenna 3D visualizer" << std::endl;
	std::cout << "V 0.1 - 2019 EA1JBP" << std::endl;

	typedef enum {
		FILE_NEC2,
		INTERACTIVE_DIPOLE,
		DEFAULT_MODE=INTERACTIVE_DIPOLE
	} OperationMode;

	OperationMode mode = DEFAULT_MODE;
	const char *cfg_file = NULL;

	for (int i = 1; i < argc; ) {
		char *param = argv[i];
		std::cout << "param #" << i << " : " << param << std::endl;
		if ( !strncmp(param, "-nec2_file", 10)) {
			if (++i < argc) {
				mode = FILE_NEC2;
				cfg_file = argv[i++];
			}
			else {
				std::cerr << "ERROR. No file cfg supplied" << std::endl;
				return -1;
			}
		}
		else if (!strncmp(param, "-dipole", 7)) {
			mode = INTERACTIVE_DIPOLE;
			if (++i < argc && argv[i][0] != '-') {
				cfg_file = argv[i++];
			}
		}
		else if (param[0] != '-') {
			// config file
			cfg_file = argv[i++];
		}
		else {
			std::cerr << "ERROR. Unknown parameter: " << param << std::endl;
			return -1;
		}
	}

	Simulation simulation;
	switch (mode) {
	case FILE_NEC2:
		simulation.loadNEC2File(cfg_file);
		break;
	case INTERACTIVE_DIPOLE:
		// to-do
		simulation.configure(new AntDipole());
		break;
	default:
		std::cerr << "ERROR. Unknown mode: " << mode << std::endl;
		return -1;
		break;
	}

#if 0
	nec_context nec;
	nec.initialize();

	c_geometry* geo = nec.get_geometry();

#  if 1
	geo->wire(0, 70, -0.048, 0.021, -0.005, 0.035, 0.043, 0.014, 0.001, 1.0, 1.0);
	geo->wire(0, 66, 0.017, -0.015, 0.014, -0.027, 0.04, -0.031, 0.001, 1.0, 1.0);
	geo->wire(0, 47, 0.046, -0.01, 0.028, -0.013, -0.005, 0.031, 0.001, 1.0, 1.0);
	geo->wire(0, 77, -0.048, -0.038, -0.04, 0.049, -0.045, -0.04, 0.001, 1.0, 1.0);
	nec.geometry_complete(0);
	nec.gn_card(-1, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	nec.ld_card(5, 0, 0, 0, 3.72e7, 0.0, 0.0);
	//	nec.pt_card(-1, 0, 0, 0);
	nec.ex_card(EXCITATION_LINEAR, 1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	nec.fr_card(0, 2, 2400.0, 100.0);
#  else
	geo->wire(0, 70, -10., 0., 10., 10., 0., 10., 0.0075, 1.0, 1.0);
	nec.geometry_complete(0);
	nec.gn_card(-1, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	nec.ld_card(5, 0, 0, 0, 3.72e7, 0.0, 0.0);
	//	nec.pt_card(-1, 0, 0, 0);
	nec.ex_card(EXCITATION_LINEAR, 1, 1, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	nec.fr_card(0, 3, 14100.0, 100.0);
#  endif

//	nec.rp_card(0, 10, 10, 0, 5, 0, 0, 0.0, 0.0, 9.0, 9.0, 0.0, 0.0);
	{
		float delta = 5.;
		nec.rp_card(0, 360/delta, 360/delta, 0, 5, 0, 0, 0.0, 0.0, delta, delta, 0.0, 0.0);
	}

	// now get the radiation pattern data. The result index is 0 since
	// this is the first (and only) radiation pattern.
	nec_radiation_pattern* rp = nec.get_radiation_pattern(0);
#endif

	osgViewer::Viewer viewer;

	// add the state manipulator
	viewer.addEventHandler(new osgGA::StateSetManipulator(viewer.getCamera()->getOrCreateStateSet()));

	// add the stats handler
	viewer.addEventHandler(new osgViewer::StatsHandler);

	SceneVisualizer *sv = new SceneVisualizer();
	sv->configure(&simulation);
	viewer.addEventHandler(sv);
	viewer.setSceneData(sv->getScene());
	viewer.getCamera()->setClearColor(BACKGROUND_COLOR);
	simulation.setSceneVisualizer(sv);

	viewer.realize();
	viewer.run();

	exit(0);
}

#include "scenegeometry.h"
#include "necparser.h"

#include <nec2pp/libnecpp.h>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/StateSetManipulator>


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

#if 1
	// Load antenna model from a NEC input file

	nec_context *nec = NULL;
	if (argc > 1) {
		char *input_filename = argv[1];
		FILE *input = fopen(input_filename, "r");
		if (!input) {
			std::cerr << "ERROR opening input file " << input_filename << std::endl;
			exit(1);
		}
		nec = create_nec_context_from_file(input_filename);
		fclose(input);
	}
	else {
		syntax(argv[0]);
		exit(1);
	}

	nec_radiation_pattern* rp = nec->get_radiation_pattern(0);
	c_geometry* geo = nec->get_geometry();

#else
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
	viewer.addEventHandler(sv);
	viewer.setSceneData(sv->buildScene(geo, rp) );

	viewer.realize();

	viewer.run();

//	nec_delete(nec);

	exit(0);
}

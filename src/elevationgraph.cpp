#include "elevationgraph.h"

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osgText/Text>
#include <osg/LineWidth>
#include <osg/LineStipple>

#include <sstream>

const std::string theFont("fonts/FreeSans.ttf");
#define PLOT_COLOR osg::Vec4(1., 0., 0., 1.)
#define PLOT_LINE_WIDTH 3.

const float outer_radius = 200.;

/**
 *
 */
ElevationGraph::ElevationGraph()
{
	_root = new osg::Group();
	_plot = new osg::Geode();

	buildFrame();

	{
		osg::MatrixTransform *trans = new osg::MatrixTransform(osg::Matrix::translate(1920 - 250, 700, 0.));
		trans->addChild(_frame);
		trans->addChild(_plot);
		_root->addChild(trans);
	}

}


/// Plot the NEC2 simulation result
void ElevationGraph::plotSimulationResult(Simulation &sim, int phi_idx, int theta_idx)
{
	nec_radiation_pattern *rp = sim.getNECRadiationPattern();

	// Clear geode
	_plot->removeDrawables(0, _plot->getNumDrawables());

	osg::Geometry *geom = new osg::Geometry();
	_plot->addDrawable(geom);
	// Vertex array
	osg::Vec3Array *vertex_array = new osg::Vec3Array();
	geom->setVertexArray(vertex_array);

	float norm_f = rp->get_normalization_factor();
	for (int i = 0; i < rp->get_ntheta(); i++) {
		float radiation_db = rp->get_power_gain(i, phi_idx) * outer_radius / norm_f;	// TO-DO: review the dB computation and visualization !!!
//		float radiation_db = std::min(0., (rp->get_power_gain(i, phi_idx) - norm_f) * outer_radius);
		radiation_db = std::max(radiation_db, 0.f);
		osg::Vec3 v = osg::Vec3(radiation_db, 0., 0.) * osg::Matrix::rotate(osg::DegreesToRadians( rp->get_theta_start() + (float)i * rp->get_delta_theta()), osg::Z_AXIS);
		vertex_array->push_back(v);
	}
	// The opposite phi angle (assuming phi turns 360º)
	float opposite_phi_idx = (phi_idx + (rp->get_nphi() / 2)) % rp->get_nphi();
	for (int i = 0; i < rp->get_ntheta(); i++) {
		float radiation_db = rp->get_power_gain(i, opposite_phi_idx) * outer_radius / norm_f;
		radiation_db = std::max(radiation_db, 0.f);
		osg::Vec3 v = osg::Vec3(-radiation_db, 0., 0.) * osg::Matrix::rotate(-osg::DegreesToRadians(rp->get_theta_start() + (float)i * rp->get_delta_theta()), osg::Z_AXIS);
		vertex_array->push_back(v);
	}

	osg::Vec4Array *colors = new osg::Vec4Array();
	colors->push_back(PLOT_COLOR);
	geom->setColorArray(colors);
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);

	osg::StateSet *sset = geom->getOrCreateStateSet();
	sset->setAttribute(new osg::LineWidth(PLOT_LINE_WIDTH));

	float half_vertices = vertex_array->size() / 2;
	geom->addPrimitiveSet(new osg::DrawArrays(osg::DrawArrays::LINE_STRIP, 0, half_vertices));
	geom->addPrimitiveSet(new osg::DrawArrays(osg::DrawArrays::LINE_STRIP, half_vertices, half_vertices));

	{
		// Dynamic labels
		osgText::Text *text = new osgText::Text();
		_plot->addDrawable(text);

		osg::Vec3 position(-220., 200., 0.);
		text->setAlignment(osgText::TextBase::LEFT_BASE_LINE);
		text->setFont(theFont);
		text->setPosition(position);
		text->setCharacterSize(20.);
		text->setFontResolution(32, 32);
		text->setColor(osg::Vec4(.1, .8, 1., 1.));
		std::stringstream info;
		info << "Azimuth: " << rp->get_phi(phi_idx) << "º" << std::endl;

#if 1
		info << "Norm factor " << rp->get_normalization_factor() << std::endl;
		info << "Avg pwr gain " << rp->get_average_power_gain() << std::endl;
		info << "Avg pwr solid angle " << rp->get_average_power_solid_angle() << std::endl;
		info << "Max gain " << rp->get_maximum_gain_db() << std::endl;
		info << "Gain max " << rp->get_gain_max() << std::endl;
		info << "Gain min " << rp->get_gain_min() << std::endl;
		info << "Gain mean " << rp->get_gain_mean() << std::endl;
		info << "Gain sd " << rp->get_gain_sd() << std::endl;
#endif

		text->setText(info.str());
	}
	{
		osg::Geometry *geom = new osg::Geometry();
		_plot->addDrawable(geom);
		// Vertex array
		osg::Vec3Array *vertex_array = new osg::Vec3Array();
		geom->setVertexArray(vertex_array);
		vertex_array->push_back(osg::Vec3(0., 0., 0.));
		vertex_array->push_back(osg::Vec3(outer_radius * 1.1, 0., 0.) * osg::Matrix::rotate(osg::DegreesToRadians(rp->get_theta(theta_idx)), osg::Z_AXIS));

		osg::Vec4Array *colors = new osg::Vec4Array();
		colors->push_back( osg::Vec4(1., 1., 1., 1.));
		geom->setColorArray(colors);
		geom->setColorBinding(osg::Geometry::BIND_OVERALL);

//		osg::StateSet *sset = geom->getOrCreateStateSet();
//		sset->setAttribute(new osg::LineWidth(PLOT_LINE_WIDTH));

		geom->addPrimitiveSet(new osg::DrawArrays(osg::DrawArrays::LINES, 0, vertex_array->size()));
	}
}

/**
 *
 */
void ElevationGraph::buildFrame()
{
	osg::Geode *geode = new osg::Geode();

	// Polar graph
	osg::Geometry *geom = new osg::Geometry();
	geode->addDrawable(geom);

	osg::Vec3Array *vertices = new osg::Vec3Array();
	geom->setVertexArray(vertices);

#define MAGNITUDE_DIVISIONS 7
	std::string db_scale[MAGNITUDE_DIVISIONS] = {
		"0",
		"-3",
		"-6",
		"-9",
		"-12",
		"-18",
		"-24"
	};
	int radial_divs = 8;	// 16 for full circle, 8 for semicircle

	// Concentric circles
	float radius = outer_radius;
	for (int c = 0; c < MAGNITUDE_DIVISIONS; c++) {
		// Circle dB indicator
		{
			osgText::Text* text = new  osgText::Text;
			geode->addDrawable(text);

			osg::Vec3 position(0.0f, radius, -1.0f);
			text->setAlignment(osgText::TextBase::CENTER_BASE_LINE);
			text->setFont(theFont);
			text->setPosition(position);
			text->setCharacterSize(15.);
			text->setFontResolution(32, 32);
			text->setText(db_scale[c]);
		}

		for (int i = 0; i <= 180; i++) {
			osg::Vec3 v = osg::Vec3(radius, 0., 0.) * osg::Matrix::rotate(osg::DegreesToRadians((float)i), osg::Z_AXIS);
			vertices->push_back(v);
		}
		radius -= outer_radius/MAGNITUDE_DIVISIONS;
		geom->addPrimitiveSet(new osg::DrawArrays(osg::DrawArrays::LINE_STRIP, c*181, 181));
	}
	for ( int line_idx = 0; line_idx <= radial_divs; line_idx++ )
	{
		// Azimuth indicator
		if ( line_idx % 2 == 0 )
		{
			osgText::Text* text = new  osgText::Text;
			geode->addDrawable(text);

			int elevation_angle = line_idx * 180 / radial_divs;
			osg::Vec3 position = osg::Vec3(outer_radius * 1.1, 0.0, -1.f) * osg::Matrix::rotate(osg::DegreesToRadians((float)elevation_angle), osg::Z_AXIS);
			text->setAlignment(osgText::TextBase::CENTER_BASE_LINE);
			text->setFont(theFont);
			text->setPosition(position);
			text->setCharacterSize(15.);
			text->setFontResolution(32, 32);
			std::stringstream aa;
			aa << elevation_angle;
			text->setText(aa.str());
		}

		osg::Vec3 v1 = osg::Vec3(outer_radius, 0., 0.);
		osg::Vec3 v2 = osg::Vec3(0., 0., 0.);
		if (line_idx == radial_divs / 2) {
			//  Make room for the dB numbers
			v1.set(outer_radius/MAGNITUDE_DIVISIONS, 0., 0.);
		}
		else if (line_idx % 2 != 0) {
			v2 = osg::Vec3(2.*outer_radius / MAGNITUDE_DIVISIONS, 0., 0.);
		}
		else if (line_idx % 4 != 0) {
			v2 = osg::Vec3(outer_radius / MAGNITUDE_DIVISIONS, 0., 0.);
		}
		// Rotate the lines
		v1 = v1 * osg::Matrix::rotate(osg::DegreesToRadians(180. / radial_divs * (float)line_idx), osg::Z_AXIS);
		v2 = v2 * osg::Matrix::rotate(osg::DegreesToRadians(180. / radial_divs * (float)line_idx), osg::Z_AXIS);
		vertices->push_back(v1);
		vertices->push_back(v2);
	}
	geom->addPrimitiveSet(new osg::DrawArrays(osg::DrawArrays::LINES, 181 * MAGNITUDE_DIVISIONS, 2 * (radial_divs+1)));

	osg::Vec4Array *colors = new osg::Vec4Array();
	colors->push_back(osg::Vec4(1., 1., 0., 1.));
	geom->setColorArray(colors);
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);

	osg::StateSet *sset = geom->getOrCreateStateSet();
	sset->setAttributeAndModes(new osg::LineStipple(1, 0x5555));

	_frame = geode;
}

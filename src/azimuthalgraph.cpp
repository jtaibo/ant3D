#include "azimuthalgraph.h"

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
AzimuthalGraph::AzimuthalGraph()
{
	_root = new osg::Group();
	_plot = new osg::Geode();

	buildFrame();

	{
		osg::MatrixTransform *trans = new osg::MatrixTransform(osg::Matrix::translate(1920 - 250, 250, 0.));
		trans->addChild(_frame);
		trans->addChild(_plot);
		_root->addChild(trans);
	}
	if ( false )
	{
		osg::MatrixTransform *trans = new osg::MatrixTransform(osg::Matrix::translate(1920 - 250, 700, 0.));
		trans->addChild(_frame);
		trans->addChild(_plot);
		_root->addChild(trans);
	}

}


/// Plot the NEC2 simulation result
void AzimuthalGraph::plotSimulationResult(Simulation &sim, int phi_idx, int theta_idx)
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
	for (int i = 0; i < rp->get_nphi(); i++) {
		float radiation_db = rp->get_power_gain(theta_idx, i) * outer_radius / norm_f;
		radiation_db = std::max(radiation_db, 0.f);
		osg::Vec3 v = osg::Vec3(0., radiation_db, 0.) * osg::Matrix::rotate(-osg::DegreesToRadians( rp->get_phi_start() + (float)i * rp->get_delta_phi()), osg::Z_AXIS);
		vertex_array->push_back(v);
	}
	osg::Vec4Array *colors = new osg::Vec4Array();
	colors->push_back(PLOT_COLOR);
	geom->setColorArray(colors);
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);

	osg::StateSet *sset = geom->getOrCreateStateSet();
	sset->setAttribute(new osg::LineWidth(PLOT_LINE_WIDTH));

	geom->addPrimitiveSet(new osg::DrawArrays(osg::DrawArrays::LINE_LOOP, 0, vertex_array->size()));

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
		info << "Elevation: " << rp->get_theta(theta_idx) << "º" << std::endl;

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
		vertex_array->push_back(osg::Vec3(-outer_radius * 1.1, 0., 0.) * osg::Matrix::rotate(osg::DegreesToRadians(rp->get_phi(phi_idx)), osg::Z_AXIS));
		vertex_array->push_back(osg::Vec3(outer_radius * 1.1, 0., 0.) * osg::Matrix::rotate(osg::DegreesToRadians(rp->get_phi(phi_idx)), osg::Z_AXIS));

		osg::Vec4Array *colors = new osg::Vec4Array();
		colors->push_back(osg::Vec4(1., 1., 1., 1.));
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
void AzimuthalGraph::buildFrame()
{
	osg::Geode *geode = new osg::Geode();

	// Polar graph
	osg::Geometry *geom = new osg::Geometry();
	geode->addDrawable(geom);

	osg::Vec3Array *vertices = new osg::Vec3Array();

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
	int radial_divs = 16;

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

		for (int i = 0; i < 360; i++) {
			osg::Vec3 v = osg::Vec3(radius, 0., 0.) * osg::Matrix::rotate(osg::DegreesToRadians((float)i), osg::Z_AXIS);
			vertices->push_back(v);
		}
		radius -= outer_radius/MAGNITUDE_DIVISIONS;
		geom->addPrimitiveSet(new osg::DrawArrays(osg::DrawArrays::LINE_LOOP, c*360, 360));
	}
	for ( int line_idx = 0; line_idx < radial_divs; line_idx++ )
	{
		// Azimuth indicator
		if ( line_idx != 0 	// First angle is 0º and already has the 0 of the decibels
			&& line_idx % 2 == 0 )
		{
			osgText::Text* text = new  osgText::Text;
			geode->addDrawable(text);

			int azimuth_angle = line_idx * 360 / radial_divs;
			osg::Vec3 position = osg::Vec3(0.0f, outer_radius * 1.1, -1.f) * osg::Matrix::rotate(-osg::DegreesToRadians((float)azimuth_angle), osg::Z_AXIS);
			text->setAlignment(osgText::TextBase::CENTER_BASE_LINE);
			text->setFont(theFont);
			text->setPosition(position);
			text->setCharacterSize(15.);
			text->setFontResolution(32, 32);
			std::stringstream aa;
			aa << azimuth_angle;
			text->setText(aa.str());
		}

		osg::Vec3 v1 = osg::Vec3(0., outer_radius, 0.);
		osg::Vec3 v2 = osg::Vec3(0., 0., 0.);
		if (line_idx == 0) {
			//  Make room for the dB numbers
			v1.set(0., outer_radius/MAGNITUDE_DIVISIONS, 0.);
		}
		if (line_idx % 2 != 0) {
			v2 = osg::Vec3(0., 2.*outer_radius / MAGNITUDE_DIVISIONS, 0.);
		}
		else if (line_idx % 4 != 0) {
			v2 = osg::Vec3(0., outer_radius / MAGNITUDE_DIVISIONS, 0.);
		}
		// Rotate the lines
		v1 = v1 * osg::Matrix::rotate(osg::DegreesToRadians(360. / radial_divs * (float)line_idx), osg::Z_AXIS);
		v2 = v2 * osg::Matrix::rotate(osg::DegreesToRadians(360. / radial_divs * (float)line_idx), osg::Z_AXIS);
		vertices->push_back(v1);
		vertices->push_back(v2);
		geom->addPrimitiveSet(new osg::DrawArrays(osg::DrawArrays::LINES, 360 * MAGNITUDE_DIVISIONS + line_idx * 2, 2* radial_divs));
	}
	geom->setVertexArray(vertices);
	osg::Vec4Array *colors = new osg::Vec4Array();
	colors->push_back(osg::Vec4(1., 1., 0., 1.));
	geom->setColorArray(colors);
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);

	osg::StateSet *sset = geom->getOrCreateStateSet();
	sset->setAttributeAndModes(new osg::LineStipple(1, 0x5555));

	_frame = geode;
}

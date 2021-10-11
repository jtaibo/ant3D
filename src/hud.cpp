#include "hud.h"
#include "utils.h"

#include <osg/Camera>
#include <osg/Geode>
#include <osg/Geometry>
#include <osgText/Text>

#include <iostream>

std::string timesFont("fonts/FreeSans.ttf");

HUD::HUD(float min_dB, float max_dB, Simulation* sim) :
	_simulation(sim)
{
	// create a camera to set up the projection and model view matrices, and the subgraph to draw in the HUD
	osg::Camera* camera = new osg::Camera;

	// set the projection matrix
	camera->setProjectionMatrix(osg::Matrix::ortho2D(0, 1920, 0, 1080));

	// set the view matrix
	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	camera->setViewMatrix(osg::Matrix::identity());

	// only clear the depth buffer
	camera->setClearMask(GL_DEPTH_BUFFER_BIT);

	// draw subgraph after main camera view.
	camera->setRenderOrder(osg::Camera::POST_RENDER);

	// we don't want the camera to grab event focus from the viewers main camera(s).
	camera->setAllowEventFocus(false);

	// turn lighting off for the text and disable depth test? to ensure it's always ontop.
	camera->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	// add to this camera a subgraph to render
	{

		osg::Geode* geode = new osg::Geode();

		{
			osg::Vec3 position(960.0f, 1000.0f, 0.0f);

			osgText::Text* text = new  osgText::Text;
			geode->addDrawable(text);

			text->setAlignment(osgText::TextBase::CENTER_BASE_LINE);
			text->setFont(timesFont);
			text->setPosition(position);
			text->setCharacterSize(20.);
			text->setText("EA1JBP Real-time 3D antenna visualizer - v0.1 2019");
		}

		camera->addChild(geode);

		camera->addChild(buildDecibelScale(min_dB, max_dB, _simulation->getNECRadiationPattern()));

	}
	_root = camera;
}

osg::Node *HUD::buildDecibelScale(float min_dB, float max_dB, nec_radiation_pattern* rp)
{
	if (!rp)
		return NULL;

	osg::Geode *geode = new osg::Geode();
	{
		{
			osg::Vec3 position(300.0f, 1000.0f, 0.0f);
			osgText::Text* text = new  osgText::Text;
			geode->addDrawable(text);

			text->setAlignment(osgText::TextBase::LEFT_BASE_LINE);
			text->setFont(timesFont);
			text->setPosition(position);
			text->setCharacterSize(20.);

			const wchar_t theta[] = L"\u03b8";
			const wchar_t phi[] = L"\u03c6";
			std::wstringstream txtstr;
			txtstr << L"Radiation pattern\nSamples:\n";
			txtstr << theta << ": " << rp->get_theta_angles().size() << " [" << rp->get_theta_start() << ", " << rp->get_theta_start() + rp->get_delta_theta() * rp->get_theta_angles().size() << "]\n"
				<< phi << ": " << rp->get_phi_angles().size() << " [" << rp->get_phi_start() << ", " << rp->get_phi_start() + rp->get_delta_phi() * rp->get_phi_angles().size() << "]\n"
				;

			if (_simulation) {
				txtstr << _simulation->getTextInfo().c_str() << std::endl;
			}

			text->setText( txtstr.str().c_str() );
		}

		// Gain dB scale
		{
			osg::Vec3 pos(50., 100., 0.);
			int divs = 50;
			float width = 50.;
			float height = 800.;
			float delta = height / (float)divs;

			osg::Geometry *geom = new osg::Geometry();

			// Vertex array
			osg::Vec3Array *vertex_array = new osg::Vec3Array();
			geom->setVertexArray(vertex_array);
			for (int i = 0; i <= divs; i++) {
				vertex_array->push_back(pos);
				pos.x() += width;
				vertex_array->push_back(pos);
				pos.x() -= width;
				pos.y() += delta;
			}

			// Color array
			osg::Vec4Array *color_array = new osg::Vec4Array();
			geom->setColorArray(color_array);
			geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
			for (int i = 0; i <= divs; i++) {
				osg::Vec4 color = pseudoColor((float)i / divs, 0., 1.);
				color_array->push_back(color);
				color_array->push_back(color);
			}

			// Primitive sets
			osg::DrawArrays *pset = new osg::DrawArrays(GL_QUAD_STRIP, 0, vertex_array->size());
			geom->addPrimitiveSet(pset);

			geode->addDrawable(geom);
		}

		// Text0
		{
			osg::Vec3 position(75.0f, 950.0f, 0.0f);
			osgText::Text* text = new  osgText::Text;
			geode->addDrawable(text);

			text->setAlignment(osgText::TextBase::CENTER_BASE_LINE);
			text->setFont(timesFont);
			text->setPosition(position);
			text->setCharacterSize(20.);
			text->setText("Gain\n(dBi)");
		}

		// TO-DO:
		// Tick marks (line and text)
		{
			float ypos = 900.;
			osg::Vec3 position(120.0f, ypos, 0.0f);
			osgText::Text* text = new  osgText::Text;
			geode->addDrawable(text);

			text->setAlignment(osgText::TextBase::LEFT_CENTER);
			text->setFont(timesFont);
			text->setPosition(position);
			text->setCharacterSize(20.);
			text->setText("+10 dBi");

			// To-do: compute roundings and scale increment, minor and major tick marks
			float from = ceilf(min_dB);
			float to = floor(max_dB);

			int num_ticks = 50;
			float width = 10.;
			float tick_vert_sep = 800. / (num_ticks-1);
			osg::Vec4 tick_color(1., 1., 1., 1.);
			for (int i = 0; i < num_ticks; i++) {
				osg::Geometry *geom = new osg::Geometry();

				// Vertex array
				osg::Vec3Array *vertex_array = new osg::Vec3Array();
				geom->setVertexArray(vertex_array);
				osg::Vec3 pos(100., 100. + tick_vert_sep * i, 0.);
				vertex_array->push_back(pos);
				pos.x() += width;
				vertex_array->push_back(pos);

				// Color array
				osg::Vec4Array *color_array = new osg::Vec4Array();
				geom->setColorArray(color_array);
				geom->setColorBinding(osg::Geometry::BIND_OVERALL);
				color_array->push_back(tick_color);

				// Primitive sets
				osg::DrawArrays *pset = new osg::DrawArrays(GL_LINES, 0, vertex_array->size());
				geom->addPrimitiveSet(pset);

				geode->addDrawable(geom);
			}
		}
	}
	return geode;
}

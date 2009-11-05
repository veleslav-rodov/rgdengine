//RGDE
#include <rgde/engine.h>

#include <rgde/render/particles/main.h>
#include <rgde/render/particles/box_emitter.h>
#include <rgde/render/particles/static_emitter.h>
#include <rgde/render/particles/spherical_emitter.h>
#include <rgde/render/particles/processor.h>
#include <rgde/render/particles/effect.h>



class CParticleTest : public game::dynamic_object
{
public:
	CParticleTest() 
		: m_debug_draw (false)
		, m_save_particles (false)
		, m_load_particles (false)
	{
		m_font = render::font::create(20, L"Arial", render::font::Heavy);

		math::vec3f eye(40, 40, -40);
		math::vec3f look_at( 0.0f, 0.0f, 0.0f );
		math::vec3f up_vec( 00.0f, 1.0f, 0.0f );

		// ������ ������
		m_effect = new particles::effect();
		//m_effect->debug_draw(m_debug_draw);
		//m_effect->debug_draw();

		if (m_load_particles)
			loadParticles();
		else
			initParticles();

		if (m_save_particles)
			saveParticles();

		// ������������� �����
		initInput();

		// ������������� ������
		m_camera  = render::render_camera::create();
		m_camera->set_projection(math::Math::PI/4, 1.0f, 1.0f, 10000.0f);
		m_target_camera = math::target_camera::create(m_camera);
		render::TheCameraManager::get().add_camera(m_camera);
		m_target_camera->set_position(eye,look_at,up_vec);
		m_target_camera->activate();
	}

	void initInput()
	{
		using namespace input;

		Input::add_command(L"Quit");
		Input::add_command(L"Horz");
		Input::add_command(L"Vert");
		Input::add_command(L"Froward");
		Input::add_command(L"Backward");
		Input::add_command(L"CW");
		Input::add_command(L"CCW");

		//m_cXAxis.attachToControl(input::Mouse, input::AxisX);
		//m_cYAxis.attachToControl(input::Mouse, input::AxisY);
		//m_cRightButton.attachToControl(input::Mouse, input::ButtonRight);
		//m_cZAxis.attachToControl(input::Mouse, input::AxisWheel);
		//m_esc.addHandler(this,&CParticleTest::onEsc);
		//m_cYAxis.addHandler(this,&CParticleTest::onYAxis);
		//m_cXAxis.addHandler(this,&CParticleTest::onXAxis);
		//m_cZAxis.addHandler(this,&CParticleTest::onWheelAxis);

		//��������� ������� � ����������
		Input::get_device(types::Keyboard)->get_control(types::KeyEscape)->bind(L"Quit");
		Input::get_device(types::Keyboard)->get_control(types::KeyW     )->bind(L"Froward");
		Input::get_device(types::Keyboard)->get_control(types::KeyS     )->bind(L"Backward");
		Input::get_device(types::Keyboard)->get_control(types::KeyE     )->bind(L"CW");
		Input::get_device(types::Keyboard)->get_control(types::KeyQ     )->bind(L"CCW");
		Input::get_device(types::Mouse   )->get_control(types::AxisX    )->bind(L"Horz");
		Input::get_device(types::Mouse   )->get_control(types::AxisY    )->bind(L"Vert");

		//������ ������� � ���������		
		//m_cR    .attach(L"Reset");
		//m_cW    .attach(L"Froward");
		//m_cS    .attach(L"Backward");
		//m_cE    .attach(L"CW");
		//m_cQ    .attach(L"CCW");
		m_keyupQuit.attach(L"Quit");
		m_cXAxis.attach(L"Horz");
		m_cYAxis.attach(L"Vert");

		//������ ��� ������ �������-�����������		
		//m_esc   += boost::bind(&TestInterpolator::onEsc,   this);
		//m_cR     += boost::bind(&TestInterpolator::on_reset, this);
		m_keyupQuit += boost::bind(&CParticleTest::onQuit, this);
		m_cYAxis += boost::bind(&CParticleTest::onYAxis, this, _1);
		m_cXAxis += boost::bind(&CParticleTest::onXAxis, this, _1);
	}

	~CParticleTest()
	{
		deleteParticles();
	}

public:
	//����� �� ���������
	void onQuit()
	{
		core::application::get()->close();
	}

	//��� X
	void onXAxis(int dx)
	{
		const int accel = 5;
		const float slow = .01f;
		const float fast = 2*slow;
		float angle = dx>accel ? dx*fast : dx*slow;

		//if (m_cRightButton)
			m_target_camera->rotateLeft(-angle);
	}

	//��� Y
	void onYAxis(int dy)
	{
		const int accel = 5;
		const float slow = .01f;
		const float fast = 2*slow;
		float angle = dy>accel ? dy*fast : dy*slow;

		//if (m_cRightButton)
			m_target_camera->rotate_up(angle);
	}

	///*onWheelAxis*/
	void onWheelAxis(int dw)
	{
		const float slow = .1f;
		m_target_camera->goBackward(-dw*slow);
	}

	//-----------------------------------------------------------------------------------
	virtual void update(float dt)
	{
		render::render_device::get().showWiredFloorGrid(100.0f, 20, math::Color(150, 150, 150, 255));
		render::render_device::get().showWiredFloorGrid(100.0f, 2, math::Color(60, 60, 60, 255));
	}
protected:
	//-----------------------------------------------------------------------------------
	void initParticles()
	{
		particles::spherical_emitter* sph_emitter = new particles::spherical_emitter();
		m_effect->add(sph_emitter);

		return;
		// ������ ��������
		for( int i = 0; i < 3; i++ )
		{
			float fDist = (float)i*10;
			// ������ ����������� ������
			particles::spherical_emitter* sph_emitter = new particles::spherical_emitter();
			m_effect->add(sph_emitter);

			particles::processor* proc = new particles::processor();
			proc->setTextureName( "particles/Shot_Smoke.png" );
			proc->setMaxParticles( 100 );
			proc->setGlobal( false );
			sph_emitter->addProcessor(proc);
			proc->load();

			sph_emitter->getTransform().set_position(math::point3f( fDist, 0, -fDist/1.732f));

			// ������ ���������� ������
			particles::box_emitter* box_emitter = new particles::box_emitter();
			m_effect->add(box_emitter);

			proc = new particles::processor();
			box_emitter->addProcessor(proc);
			proc->setTextureName( "particles/Shot_Smoke.png" );
			proc->setMaxParticles( 100 );
			proc->setGlobal( false );
			proc->load();

			box_emitter->getTransform().set_position( math::point3f( -fDist, 0, -fDist/1.732f) );

			// ������ ���� ������
			particles::static_emitter* static_emitter = new particles::static_emitter("particles/cannonShot_smoke.prt", "particles/shot_smoke.png");
			m_effect->add( static_emitter );

			static_emitter->setCycling(true);
			static_emitter->setVisible(true);

			static_emitter->getTransform().set_position( math::point3f(0, 0, fDist ) );
		}
	}

	//-----------------------------------------------------------------------------------
	void deleteParticles()
	{
		delete m_effect;
		particles::static_emitter::clear_cached_data();
		render::effect::clear_all();
	}

	//-----------------------------------------------------------------------------------
	void loadParticles()
	{
		//io::ReadFile in( 
		//	io::helpers::createFullFilePathA(L"Media/particles/particles_serialization.dat").c_str() );
		//if (!in.is_open())
		//{
			initParticles();
			//saveParticles();
			//deleteParticles();

			// ������ ������
			//m_effect = new particles::IEffect();
			//m_effect->setDebugDraw(true);
		//	in.open( io::helpers::createFullFilePathA(L"Media/particles/particles_serialization.dat").c_str() );
		//}
		//in >> *m_effect;
	}

	//-----------------------------------------------------------------------------------
	void saveParticles()
	{
		//io::write_file out( 
		//	io::helpers::createFullFilePathA(L"Media/particles/particles_serialization.dat").c_str() );
		//out << *m_effect;
	}


protected:
	//������ ��� �����
	input::KeyUp        m_keyupQuit;
	input::RelativeAxis m_cXAxis;
	input::RelativeAxis m_cYAxis;
	input::KeyUp       m_esc;
	input::KeyUp		m_cRightButton;
	input::RelativeAxis m_cZAxis;

	math::camera_ptr			m_camera;
	math::target_camera_ptr		m_target_camera;      //���������� ������ "���������� ������"

	render::font_ptr		m_font;

	bool m_debug_draw;				// ����� �� ��������� � ����� �������� ���������
	bool m_save_particles;			// ����� �� ��������� ������ ������ � ����
	bool m_load_particles;			// ����� �� ��������� ������ ������ ���������� ��� ������� �� �����
	particles::effect*	m_effect;	// ������ ������
};

int main()
{
	std::auto_ptr<core::application> app(core::application::create(L"Test Particles Example", 800, 600, false));

	app->add<core::input_task>(0);
	app->add<core::game_task>(1);
	app->add<core::render_task>(2);


	{
		CParticleTest particleTest;
		app->run();
	}

	return 0;
}
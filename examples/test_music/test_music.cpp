#include <rgde/engine.h>

#include <windows.h>

struct TextQuad
{
	std::wstring text;
	math::Rect rect;
	math::Color color;
};


class TestMusic :  public game::dynamic_object
{
public:
	TestMusic();

protected:
	void init_input();

	void update (float dt);

	void NextMusic ();

	void onEsc ();

	TextQuad CreateQuad(std::wstring text);

protected:
	int cur_music_index;
	render::font_ptr	m_font;
	render::camera_ptr	m_camera;
	input::key_down		m_esc;
	input::key_down		m_cSpace;
	audio::system		m_sound_system;

	input::Mouse           m_mouse;
	std::vector<TextQuad> quads;
};


//
//class CSound3dTest : public game::dynamic_object
//{
//public:
//	CSound3dTest()
//	{
//		CoInitialize(0);
//
//		m_pSound3d = sound::CSound3d::Create (L"helicopter.wav");
//		m_pSound3d->set_pos (math::vec3f (0, 10, 0));
//		m_pSound3d->setLoop (true);
//		m_pSound3d->play();
//
//		sound::Sound3dManager::get().set_pos (math::vec3f (0, 0, 0));
//	}
//
//	~CSound3dTest()
//	{
//		CoUninitialize();
//	}
//
//	void update (float dt)
//	{
//		static float angle = 0.0f;
//		angle += math::Math::PI / 100;
//
//		m_pSound3d->set_pos (math::vec3f ( 100.0f + 100.0f*cosf(angle), 0, 100.0f*sinf(angle) ));
//
//		sound::Sound3dManager::get().update (dt);
//	}
//
//protected:
//	sound::Sound3dPtr	m_pSound3d;							///< 3d Sound
//};
//

// The application's entry point
int __stdcall WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, int )
{
	using namespace core;

	typedef std::auto_ptr<application> app_ptr;

	 app_ptr app(application::create(L"Test Music", 640, 480, false));
	

	app->add<render_task>(0);
	app->add<input_task>(1, false);
	app->add<game_task>(2);
//
	TestMusic r;
//	SimpleInput i;
//	CSound3dTest test;
////	CMusicTest test;
//
	app->run();
}

//////////////////////////////////////////////////////////////////////////

TestMusic::TestMusic() 
: m_sound_system(core::application::get()->get_handle())
, cur_music_index(0)
{
	m_sound_system.load("./media/audiodb.xml");

	m_font = render::font::create(20, L"Arial", render::font::Heavy);

	m_camera = render::render_camera::create();

	math::vec3f eye( 0.0f, 0, -50 );
	math::vec3f look_at( 0.0f, 0.0f, 0.0f );
	math::vec3f up_vec( 0.0f, 1.0f, 0.0f );

	m_camera->look_at(eye, look_at, up_vec);
	m_camera->set_projection(math::Math::PI/4, 1.0f, 1.0f, 10000.0f);

	init_input();

	render::TheCameraManager::get().add_camera(m_camera);

	m_sound_system.play(cur_music_index);

	for (size_t t = 0; t < 50; ++t)
		quads.push_back(CreateQuad(L"Bla-blajh"));
}

void TestMusic::init_input()
{
	using namespace input;

	m_cSpace.attach(L"NextMusic");
	Input::get_device(types::Keyboard)->get_control(types::KeySpace)->bind(L"NextMusic");
	m_cSpace += boost::bind(&TestMusic::NextMusic, this);

	m_esc.attach(L"Quit");
	Input::get_device(types::Keyboard)->get_control(types::KeyEscape)->bind(m_esc.getCommand());
	m_esc += boost::bind(&TestMusic::onEsc, this);
}


void TestMusic::update (float dt)
{
	using render::font;

	for (size_t i = 0; i < quads.size(); ++i)
	{
		m_font->render(quads[i].text, quads[i].rect, quads[i].color.color, false, font::SingleLine | font::VCenter);
	}

	float abs_time = game::game_system::get().get_timer().get_absolute_time();
	render::render_device::get().draw_fps(abs_time, m_font);


	unsigned int flags = font::SingleLine | font::Bottom;
	math::vec2f size(800, 600);

	const char *cur_music_name = m_sound_system.get_tag_name(cur_music_index);

	std::string s(cur_music_name);
	std::wstring ws(s.begin(), s.end());
	m_font->render(ws, math::Rect(0, 0, 800, 600), 0xFFFFFFFF, true, flags);

	m_sound_system.update((int)dt*1000);
}

void TestMusic::NextMusic ()
{
	cur_music_index++;
	cur_music_index %= m_sound_system.get_num_tags();
	m_sound_system.stop_all();
	m_sound_system.play(cur_music_index);
}

void TestMusic::onEsc ()
{
	core::application::get()->close();
}

TextQuad TestMusic::CreateQuad(std::wstring text)
{
	TextQuad q;
	q.text = text;
	q.color = math::Color(255*math::unitRandom(),255*math::unitRandom(),255*math::unitRandom(), 255);

	q.rect = m_font->get_rect(q.text, render::font::SingleLine | render::font::VCenter);		
	math::vec2f size(800-40-q.rect.size[0], 600-40-q.rect.size[1]);

	float x = 20+math::unitRandom()*size[0];
	float y = 20+math::unitRandom()*size[1];

	q.rect.position = math::vec2f(x, y);

	return q;
}
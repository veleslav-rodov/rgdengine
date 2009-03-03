//RGDE
#include <rgde/engine.h>
#include "TriangleManager.h"

#include <boost/scoped_ptr.hpp>

class HelloMessage :  public game::dynamic_object
{
public:
	HelloMessage() :  
	  spApp(core::application::create(L"Test_Lines Example", 800, 600, false, false)),
	  m_TrianglesManager(9)
	{
		spApp->addTask(core::task_ptr(new core::render_task(*spApp, 1)));
		spApp->addTask(core::task_ptr(new core::input_task(*spApp,0)));
		spApp->addTask(core::task_ptr(new core::game_task(*spApp,2)));

		m_pFont = render::IFont::create(20, L"Arial", render::IFont::Heavy);

		m_camera = render::render_camera::create();

		math::Vec3f vEyePt( 0.0f, 0, -50 );
		math::Vec3f vLookatPt( 0.0f, 0.0f, 0.0f );
		math::Vec3f vUpVec( 0.0f, 1.0f, 0.0f );

		m_camera->lookAt(vEyePt, vLookatPt, vUpVec);
		m_camera->setProjection(math::Math::PI/4, 1.0f, 1.0f, 10000.0f);

        {
            using namespace input;

            m_cEsc.attach(L"Quit");
			Input::getDevice(types::Keyboard)->get_control(types::KeyEscape)->bind(L"Quit");
            
            m_cEsc += boost::bind(&HelloMessage::onEsc, this);
        }

		render::TheCameraManager::get().addCamera(m_camera);
		
		spApp->Run();
	}

protected:
	void update (float dt)
	{
		render::TheDevice::get().showFPS(m_pFont);
		m_TrianglesManager.update();
		m_TrianglesManager.render();
	}

	void onEsc ()
	{
		core::application::get()->close();
	}

protected:
	render::font_ptr                         m_pFont;
	render::camera_ptr                       m_camera;
	lines_test::CTriangleManager          m_TrianglesManager;
	boost::scoped_ptr<core::application> spApp;
	input::KeyDown                       m_cEsc;
};

// The application's entry point
int __stdcall WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, int )
{
	HelloMessage r;
}
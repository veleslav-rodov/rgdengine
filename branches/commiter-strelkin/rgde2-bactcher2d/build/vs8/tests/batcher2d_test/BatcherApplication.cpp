#include "stdafx.h"

#include "BatcherApplication.h"


namespace rgde
{
	BatcherApplication::BatcherApplication(int x, int y, int w, int h, const std::wstring& title)
		:m_active(true),
		window(math::vec2i(x, y), math::vec2i(w, h), title, 0, WS_BORDER | WS_CAPTION | WS_SYSMENU),
		m_device(get_handle()), m_batcher(0)
	{
		show();
		update();
		m_batcher = new render::renderer_2d(m_device);
	}

	BatcherApplication::~BatcherApplication()
	{
		if(m_batcher)
			delete m_batcher;
	}

	void BatcherApplication::run()
	{
		while( is_created() )
		{
			if( !do_events() && m_active)
			{
				m_batcher->render_all();
			}
		}
	}

	void BatcherApplication::render_frame()
	{
	}

	void BatcherApplication::add_sprite(rgde::render::primitives_2d::sprite_desc &sprite)
	{
		m_batcher->add_sprite(sprite);
	}

	void BatcherApplication::init_render_data()
	{
	}

	core::windows::result BatcherApplication::wnd_proc(ushort message, uint wparam, long lparam )
	{
		switch (message)
		{
		case WM_ACTIVATE:	// Watch For Window Activate Message
			{
				if (!HIWORD(wparam))// Check Minimization State
					m_active = true;
				else
					m_active = false;

				return 0;
			}
		case WM_KEYDOWN:
			{
				return 0;
			}
			break;
		case WM_SIZE:
			{
				return 0;
			}
			break;

		case WM_MOUSEMOVE:
			break;
		}
		return window::wnd_proc(message, wparam, lparam);
	}

	bool BatcherApplication::do_events()
	{
		MSG msg = {0};
		if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
			return true;
		}
		return false;
	}
}
#include "precompiled.h"

#include <rgde/render/particles/main.h>
#include <rgde/render/particles/effect.h>

// ���� �������
#include <rgde/render/particles/static_emitter.h>
#include <rgde/render/particles/tank.h>

// ����������� ��������
#include <rgde/render/particles/box_emitter.h>
#include <rgde/render/particles/spherical_emitter.h>

namespace particles
{
	//-----------------------------------------------------------------------------------
	effect::effect()
	: render::rendererable(9)
	, old_time(0)
	, m_is_fading(false)
	, m_transform(math::frame::create())
	//, core::meta_class("ParticleEffect")
	{	
		m_renderInfo.render_func		= boost::bind( &effect::render, this );
		m_renderInfo.debug_render_func	= boost::bind(&effect::debug_draw, this);
		
		// public properties:
		//REGISTER_PROPERTY(Transform, math::frame)
	}
	//-----------------------------------------------------------------------------------
	effect::~effect()
	{
		for (emitters_iter it = m_emitters.begin(); it != m_emitters.end(); ++it)
		{
			delete( *it );
		}
		m_emitters.clear();
	}
	//-----------------------------------------------------------------------------------
	void effect::reset()
	{
		m_is_fading = false;
		set_emitters_to_fade(m_is_fading);

		for (emitters_iter it = m_emitters.begin(); it != m_emitters.end(); ++it)
			(*it)->reset();
	}
	//-----------------------------------------------------------------------------------
	void effect::render()
	{
		for (emitters_iter it = m_emitters.begin(); it != m_emitters.end(); ++it)
			(*it)->render();
	}
	//-----------------------------------------------------------------------------------
	void effect::set_emitters_to_fade(bool b)
	{
		for(emitters_iter it = m_emitters.begin(); it != m_emitters.end(); ++it)
			(*it)->set_fade(b);
	}
	//-----------------------------------------------------------------------------------
	void effect::update(float fDeltaTime)
	{
		static float dt = 0.02f;

		{ // �������� ������ ������ 25 ��� � �������
			dt += fDeltaTime;
			if (dt < 0.02f) return;
		}
		
		for (emitters_iter it = m_emitters.begin(); it != m_emitters.end(); ++it)
			(*it)->update(dt);

		dt = 0;
	}

	//-----------------------------------------------------------------------------------
	void effect::add(emitter* em)
	{
		assert(0 != em);
		m_emitters.push_back(em);
		m_transform->add(em->get_transform());
	}
	
	//-----------------------------------------------------------------------------------
	void effect::remove(emitter* em)
	{
		assert(0 != em);
		m_transform->remove(em->get_transform());		
		m_emitters.remove(em);
	}

	//-----------------------------------------------------------------------------------
	void effect::debug_draw()
	{
		m_transform->debug_draw();
		for( emitters_iter it = m_emitters.begin(); it != m_emitters.end(); ++it )
			(*it)->debug_draw();
	}
	//-----------------------------------------------------------------------------------
	void effect::to_stream(io::write_stream& wf)
	{
		wf << file_version;
		wf << m_transform;

		// ��������� ����������� �������
		wf << (unsigned)m_emitters.size();
		for( emitters_iter it = m_emitters.begin(); it != m_emitters.end(); it++ )
			wf << *(*it);
	}
	//----------------------------------------------------------------------------------
	render::renderable_info& effect::get_renderable_info()
	{
		return m_renderInfo;		
	}
	//-----------------------------------------------------------------------------------
	void effect::from_stream(io::read_stream& rf)
	{
		unsigned version;
		rf >> version;
		if (version != file_version)
			throw std::exception("pfx::effect::from_stream(...): Unknown version !");
		
		rf >> *m_transform;

		unsigned size;
		rf >> size;
		for(unsigned i = 0; i < size; i++)
		{
			unsigned em_type = 0;
			rf >> em_type;
			emitter::Type type = static_cast<emitter::Type> (em_type);
		
			emitter* em;
			switch(type)
			{
				case emitter::Static:		em = new static_emitter;	break;
				case emitter::Spherical:	em = new spherical_emitter;	break;
				case emitter::Box:			em = new box_emitter;		break;
			}
			rf >> (*em);
			add(em);
		}
	}
}
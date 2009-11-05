#include "precompiled.h"

#include <rgde/render/particles/emitter.h>
#include <rgde/io/io.h>

namespace particles
{
	emitter::emitter(Type _type) : m_type(_type)//, core::meta_class("ParticleEmitter") 
	{
	}

	//-----------------------------------------------------------------------------------
	emitter::~emitter()
	{
		m_transform.get_parent();
	}

	//-----------------------------------------------------------------------------------
	void emitter::to_stream (io::write_stream& wf) const
	{
		wf	<< (unsigned)m_type
			<< m_transform;
	}

	//-----------------------------------------------------------------------------------
	void emitter::from_stream (io::read_stream& rf)
	{
		rf	>> m_transform;
	}
}
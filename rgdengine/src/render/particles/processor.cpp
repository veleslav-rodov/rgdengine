#include "precompiled.h"

#include <rgde/render/particles/main.h>
#include <rgde/render/particles/processor.h>
#include <rgde/render/particles/tank.h>
#include <rgde/render/particles/abstract_emitter.h>


namespace particles{

	//-----------------------------------------------------------------------------------
	processor::processor(base_emitter* em )   // �����������
	: m_is_visible(true), m_parent_emitter(em), m_is_geometric(false), core::meta_class("processor")
	{
		m_is_anim_texture_used = false; 		
		m_is_anim_texture_cycled = false;		
		m_texture_fps		= 25;		
		m_rnd_seed			= 0;		
		m_is_sparks			= false;		
		m_is_geometric		= false;
		m_dt				= 0;
		m_rate_accum		= 0;
		m_is_fading			= false;
		first_time_init();
		m_parent_emitter	= 0;
		m_is_global			= true;

		m_ngkx = 1.8f;

		m_rnd_frame			 = 0;
		m_is_play_tex_anim = true;

		m_tank = NULL;

		m_color_alpha.add_key (0, math::Color (255, 255, 255, 255));
		m_color_alpha.add_key (1, math::Color (0, 0, 0, 0));
		m_life.add_key(1, 1.0f);
		m_size.add_key(1, 1.0f);
		m_rate.add_key(1, 10.0f);
		m_vel_spread_amp.add_key(1, 1.0f);

		//addProperty(new property<bool>(m_is_anim_texture_used, "AnimTextureUse", "bool"));
		//addProperty(new property<bool>(m_is_anim_texture_cycled, "TexAnimCycled", "bool"));
		//addProperty(new property<unsigned>(m_texture_fps, "TexFps", "int"));
		//addProperty(new property<int>(m_rnd_seed, "RndSeed", "int"));
		//addProperty(new property<bool>(m_is_sparks, "IsSparks", "bool"));
		//addProperty(new property<bool>(m_is_geometric, "IsGeometric", "bool"));
		//addProperty(new property<bool>(m_is_global, "IsGlobal", "bool"));

		//addProperty(new property<math::FloatInterp>(m_rate, "PRate", "FloatInterp"));
		//addProperty(new property<math::FloatInterp>(m_PResistance, "PResistance", "FloatInterp"));
		//addProperty(new property<math::FloatInterp>(m_PSpin, "PSpin", "FloatInterp"));
		//addProperty(new property<math::FloatInterp>(m_PSpinSpread, "PSpinSpread", "FloatInterp"));
		//addProperty(new property<math::FloatInterp>(m_life, "PLife", "FloatInterp"));
		//addProperty(new property<math::FloatInterp>(m_PLifeSpread, "PLifeSpread", "FloatInterp"));
		//addProperty(new property<math::FloatInterp>(m_size, "PSize", "FloatInterp"));
		//addProperty(new property<math::ColorInterp>(m_color_alpha, "PColorAlpha", "ColorInterp"));
		//addProperty(new property<math::Vec3Interp>(m_PActingForce, "PActingForce", "Vec3Interp"));
		//addProperty(new property<math::Vec3Interp>(m_velocity, "PVelocity", "Vec3Interp"));
		//addProperty(new property<math::Vec3Interp>(m_PInitialVelSpread, "PInitialVelSpread", "Vec3Interp"));
		//addProperty(new property<math::FloatInterp>(m_vel_spread_amp, "PVelSpreadAmplifier", "FloatInterp"));
	
		// public properties:
		//REGISTER_PROPERTY(bIsAnimTextureUsed,	bool)
		//REGISTER_PROPERTY(bIsTexAnimCycled,		bool)
		//REGISTER_PROPERTY(cTexFps,				unsigned int)
		//REGISTER_PROPERTY(iRndSeed,				int)
		//REGISTER_PROPERTY(bIsSparks,			bool)
		//REGISTER_PROPERTY(bIsGeometric,			bool)
		//REGISTER_PROPERTY(bIsGlobal,			bool)

		//REGISTER_PROPERTY(PRate,				math::FloatInterp)
		//REGISTER_PROPERTY(PResistance,			math::FloatInterp)
		//REGISTER_PROPERTY(PSpin,				math::FloatInterp)
		//REGISTER_PROPERTY(PSpinSpread,			math::FloatInterp)
		//REGISTER_PROPERTY(PLife,				math::FloatInterp)
		//REGISTER_PROPERTY(PLifeSpread,			math::FloatInterp)
		//REGISTER_PROPERTY(PSize,				math::FloatInterp)
		//REGISTER_PROPERTY(PColorAlpha,			math::ColorInterp)
		//REGISTER_PROPERTY(PActingForce,			math::Vec3Interp)
		//REGISTER_PROPERTY(PVelocity,			math::Vec3Interp)
		//REGISTER_PROPERTY(PInitialVelSpread,	math::Vec3Interp)
		//REGISTER_PROPERTY(PVelSpreadAmplifier,	math::FloatInterp)
		//REGISTER_PROPERTY(TexName,				std::string)
	}

	//-----------------------------------------------------------------------------------
	processor::~processor()
	{
		delete m_tank;
	}

	//-----------------------------------------------------------------------------------
	void processor::updateParticle(particle& p)
	{
		// ����������� ������� ������� �� �������� �� 0 �� 1 
		float t = p.time / p.ttl; 
		// time passed from last update
		float dt = p.time - p.old_time;

		if (t >= 1){
			p.dead = true;
			p.color.a = 0;
			return;
		}

		p.color = m_color_alpha(t);

		math::vec3f velocity = m_velocity(t);
		
		// setGlobal acceleration
		//const math::vec3f &acceleration = m_parent_emitter->getAcceleration(m_is_global);
		const math::vec3f force = m_PActingForce(m_fNormalizedTime);

		p.vel += dt * (/*acceleration + */force * 0.5f / p.mass);
		

		// initial velocity
		velocity += p.vel;
		// setGlobal velocity
		// velocity += m_parent_emitter->getGlobalVelocity(m_is_global);

		float resistance = math::length(m_fScaling) * math::length(m_fScaling) * (m_PResistance(t) + 1.0f);
		float vel_spread_amp = m_vel_spread_amp(t);

		velocity = math::length(m_fScaling) * math::length(m_fScaling) * (p.vel_spread * vel_spread_amp + velocity ) / resistance;
			
		p.sum_vel = velocity * dt;

		p.pos += p.sum_vel;
		        
		p.size = math::length(m_fScaling)*math::length(m_fScaling) * m_size(t);

		p.rot_speed = m_PSpin(t);
		p.rotation += p.rot_speed + p.initial_spin;

		// save current update time
		p.old_time = p.time;

		//m_texture_fps
	}

	//-----------------------------------------------------------------------------------
	void processor::reset()								// ReStart ��������� - �������� ��� ����� �� 0
	{
		m_is_fading = false;

		unsigned size = static_cast<unsigned>(m_Particles.size());
		particle* pp = &(m_Particles[0]);//[i]

		for (unsigned i = 0; i < size; ++i)
		{
			pp[i].dead = true;
			pp[i].size = 0.0f;
		}
	}

	//-----------------------------------------------------------------------------------
	void processor::first_time_init()
	{
		m_is_anim_texture_used		= false;
		m_is_global				= true;
		m_bModifiersLoaded		= false;
		m_bIsPtankInited		= false;
		m_is_visible			= true;
		m_bIntense				= false;
		
		m_nMaxParticles			= 0;
		m_fNormalizedTime		= 0;
		
		//m_texture_name = clx::cstr("p_default.bmp");
	}

	//-----------------------------------------------------------------------------------
	void processor::render()
	{
		if( !m_is_visible )
			return;

		formTank();

		if (!m_is_geometric)
		{
			m_tank->render(m_texture, m_parent_emitter->getTransform());
		}
		else
			geomRender();
	}

	//-----------------------------------------------------------------------------------
	void processor::geomRender()
	{
	}
	//-----------------------------------------------------------------------------------
	void processor::update(float dt) 
	{
		/*
		if (m_rnd_seed){
			rnd.set_seed(m_rnd_seed);
		}
		*/

		m_dt += dt;

		m_fNormalizedTime = m_parent_emitter->getTime();

		math::frame& rParentTransform = m_parent_emitter->getTransform();
		//m_fScaling = (m_ParentTransform->getScaling()).x;
		m_fScaling = rParentTransform.get_scale();

		int m_acting_particles = 0;
		// ����� ���������� ������ ���������
		//for (particles_iter it = m_Particles.begin(); it != m_Particles.end(); ++it)
		//	if (!(it->dead) && m_is_global)
		//	{
		//		it->pos += m_ngkx * m_parent_emitter->m_vCurDisplacement * (1.0f - (it->time / it->ttl));
		//	}

		if (m_dt > 0.01f)
		{
			// ����� ���������� ������ ���������
			for (particles_iter it = m_Particles.begin(); it != m_Particles.end(); ++it)
				if (!(it->dead))// && m_is_global)
				{
					++m_acting_particles;
					it->time += m_dt;
					updateParticle(*it);
				}

			m_dt = 0;
			// �������� ����� �������� ���� ����(���� ���� ����� � �����)
			// ���� � ���� ��� ���� (���-�� Rate)
			if (!m_is_fading)
				addNewParticles(m_nMaxParticles - m_acting_particles);
		}
	}

	//-----------------------------------------------------------------------------------
	void processor::addNewParticles(int num2add)
	{
		// ��� ����� ��� ����� ���������
		if(!num2add) 
			return;

		// �������� ��������� ��������� ���=��\�� �������
		float rate = m_rate.getValue(m_fNormalizedTime) / 25.0f;
		m_rate_accum += rate;

		int to_add  = (int)m_rate_accum;

		if (to_add > num2add) 
			to_add = num2add;
		
		int added_num = 0;

		for(int i = 0; i < m_nMaxParticles; ++i)
			if (m_Particles[i].dead && to_add > 0){
				createParticle(m_Particles[i]);
				to_add--;
				added_num++;
			}
			else if(to_add < 1)
				break;
		m_rate_accum -= added_num;
	}

	//-----------------------------------------------------------------------------------
	void processor::createParticle(particle& p)
	{
		m_parent_emitter->get_particle(p);
		p.dead = false;
		
		p.ttl = (m_life.getValue(m_fNormalizedTime)
			+ rnd()* m_PLifeSpread.getValue(m_fNormalizedTime));// * 10.0f; // debug

		if (!p.ttl)
			p.dead = true;

		math::vec3f ivs = m_PInitialVelSpread.getValue(m_fNormalizedTime);

		p.vel_spread = math::vec3f(ivs[0] * (rnd()*2.0f - 1.0f),
							ivs[1] * (rnd()*2.0f - 1.0f),
							ivs[2] * (rnd()*2.0f - 1.0f));

		p.initial_spin = (rnd() * m_PSpinSpread.getValue(m_fNormalizedTime))
						/(25.0f);
		
		if (m_is_global){
			math::xform( p.initial_pos, getLTM(), p.initial_pos );
			p.initial_vel = m_parent_emitter->getSpeed();// / 2.0f;
		}

		p.pos = p.initial_pos;
		
		p.old_time = 0.0f;

		static math::unit_rand_2k lrnd;

		if (m_is_play_tex_anim)
		{
			float f = (float)m_rnd_frame;
			p.cur_tex_frame = lrnd() * f;

			if (p.cur_tex_frame  > m_ucTexFrames)
				p.cur_tex_frame = (float)m_ucTexFrames;
		}
		else
		{
			p.cur_tex_frame = lrnd() * (float)m_ucTexFrames;
		}

		updateParticle(p);
	}

	//-----------------------------------------------------------------------------------
	void processor::debug_draw()
	{
		if( !m_is_visible )
			return;

		math::matrix44f m = getLTM();

		if (m_is_global)
			m = math::setTrans( m, math::vec3f(0,0,0) );

		math::vec3f center, vel;
		render::lines3d& line_manager = render::render_device::get().get_lines3d(); 
		for (particles_iter it = m_Particles.begin(); it != m_Particles.end(); ++it)
		{
			if ((*it).dead)
				continue;

			if (!m_is_global)
			{
				center = m * (math::point3f)((*it).pos);
				vel = it->pos + m * (math::point3f)((*it).sum_vel*5.0f);
			}
			else
			{
				center = (*it).pos;
				vel = it->pos + (*it).sum_vel*5.0f;
			}

			line_manager.add_quad( center, math::vec2f (it->size, it->size), 0 );	
			line_manager.add_line( center, vel, math::Green );
		}
	}
	//-----------------------------------------------------------------------------------
	void processor::formTank()
	{
		if (m_is_geometric)
			return;

		const math::matrix44f& ltm = getLTM();

		renderer::ParticleArray& array = m_tank->getParticles();
		array.resize( m_Particles.size() );
		int i = 0;

		for (particles_iter it = m_Particles.begin(); it != m_Particles.end(); ++it)
		{
			particle &p = *it;

			//if (m_is_global)
			//	//*poss = p.pos;
			//	array[i].pos = p.pos;
			//else{
			//	//*poss = ltm.transform(p.pos);
			//	math::xform( array[i].pos, ltm, p.pos );
			//}
			array[i].pos = p.pos;


			array[i].size = math::vec2f( p.size, p.size );
			array[i].spin = p.rotation * 3.1415926f/180.0f;
			array[i].color = p.color.color;

			//if (m_is_anim_texture_used)
			//{
			//	agl::AglTexCoords uv1, uv2;

			//	unsigned char frame = 0;

			//	if (m_is_play_tex_anim)
			//	{
			//		if (!m_is_anim_texture_cycled)
			//			frame = (unsigned char)((m_ucTexFrames - 1) * (p.time / p.ttl));
			//		else 
			//			frame = (unsigned char)(p.time * m_texture_fps);
			//	}

			//	frame += (unsigned char)p.cur_tex_frame;

			//	if (frame > m_ucTexFrames)
			//	{
			//		if (!m_is_anim_texture_cycled)
			//			frame = m_ucTexFrames;
			//		else
			//			frame = frame % m_ucTexFrames;
			//	}

			//	int x = (frame - 1) / m_ucCol;
			//	int y = frame - m_ucCol * x;
			//	x += 1;
			//	if (y == 0) y = 1;

			//	// proskakivaet posledniy ryad - ya tormoz

			//	uv1.tu = (x - 1) * 1.0f / m_ucTrow;//m_ucRow;
			//	uv1.tv = (y - 1) * 1.0f / m_ucTcol;//m_ucCol;

			//	uv2.tu = x * 1.0f / m_ucTrow;//m_ucRow;
			//	uv2.tv = y * 1.0f / m_ucTcol;//m_ucCol;

			//	//uv1.tv = (x - 1) * 1.0f / m_ucTcol; uv1.tu = (y - 1) * 1.0f / m_ucTrow;
			//	//uv2.tv = x * 1.0f / m_ucTcol; uv2.tu = y * 1.0f / m_ucTrow;

			//	(*uvs).v[0] = uv1;
			//	(*uvs).v[1] = uv2;
			//}
			//else 
			//{
			//	static agl::AglTexCoords suv[2] = {{0,0},{1,1}};
			//	(*uvs).v[0] = suv[0];
			//	(*uvs).v[1] = suv[1];
			//}				
				i++;
		}
		m_tank->update();
	}

	//-----------------------------------------------------------------------------------
	void processor::loadTexture()
	{
		//try{
		//	agl::LocalImgPathSync p(pfx::tex_path.c_str());//"Effects\\tx\\");
		//	
		//	m_texture.load(m_texture_name);

		//	std::string ext_path = std::string::Concat(
		//		pfx::tex_path.c_str(), pfx::tex_path.length(),
		//		 m_texture_name.c_str(), m_texture_name.length() - 3);
		//	ext_path += "tnf";

		//	if (base::ResourceMaster::get()->isResourceExist(ext_path))
		//	{
		//		clx::rstream in(base::ResourceMaster::get()->getResource(ext_path));//ResourseMaster
		//		//m_texture.load(clx::cstr("anim_explode.png"));
		//		m_is_anim_texture_used = true;
		//		in  >> m_ucRow			// = 4;
		//			>> m_ucCol			// = 4;
		//			>> m_ucTexFrames		// = 16;
		//			>> m_ucTrow
		//			>> m_ucTcol;
		//	}
		//	else 
		//		m_is_anim_texture_used = false;

		//	m_texture.setFilterMode(agl::filter_Linear);
		//}
		//catch (...){
		//	m_texture_name.clear();
		//	return;
		//}
		m_texture = render::texture::create(m_texture_name);//std::wstring(m_texture_name.begin(), m_texture_name.end()) );
		return;
	}

	//-----------------------------------------------------------------------------------
	void processor::setTextureName( const std::string& texName )
	{
		m_is_texture_loaded = false;

		if ( texName.length() != 0 )
		{
			m_texture_name = texName;	
			loadTexture();
			if( !m_tank )
				return;

			m_is_texture_loaded = true;
		}
	}

	//-----------------------------------------------------------------------------------
	const math::matrix44f& processor::getLTM()
	{
		return m_parent_emitter->getTransform().get_full_tm();
	}

	//-----------------------------------------------------------------------------------
	void processor::load()
	{
		reset();
		initPTank();
		setIntenseMode(m_bIntense);
	}

	//-----------------------------------------------------------------------------------
	void processor::initPTank()
	{
		if (!m_nMaxParticles) return;

		if( m_tank ) 	delete m_tank;

		m_tank = new renderer();


		if( m_texture_name.length() )
			// TODO: ���������, ��������� �� ��������
			if( !(m_texture /*&& m_texture->isLoaded()*/ ) )
				loadTexture();

		setIntenseMode(m_bIntense);

		m_bIsPtankInited = true;

		return;
	}

	//-----------------------------------------------------------------------------------
	void processor::assignChilds()
	{
		m_bModifiersLoaded = true;
	}

	//-----------------------------------------------------------------------------------
	void processor::to_stream(io::write_stream& wf) const
	{
		wf  << file_version
			<< m_bIntense
			<< m_fScaling
			<< m_is_global
			<< m_nMaxParticles
			<< m_texture_name
			<< m_is_sparks
			//<< m_DffName
			<< m_rnd_seed
			<< m_ngkx
			<< m_texture_fps
			<< (m_is_anim_texture_cycled)
			<< (m_rate)
			<< (m_PResistance)
			<< (m_PSpin)
			<< (m_PSpinSpread)
			<< (m_life)
			<< (m_PLifeSpread)
			<< (m_size)
			<< (m_color_alpha)
			<< (m_PActingForce)
			<< (m_velocity)
			<< (m_PInitialVelSpread)
			<< (m_vel_spread_amp)
			<< m_rnd_frame
			<< m_is_play_tex_anim;
	}

	void processor::from_stream(io::read_stream& rf)
	{
		unsigned version;
		rf  >> version;
		if( version != file_version )
			assert(false);

		rf	>> m_bIntense
			>> m_fScaling
			>> m_is_global
			>> m_nMaxParticles
			>> m_texture_name
			>> m_is_sparks
			//>> m_DffName
			>> m_rnd_seed
			>> m_ngkx
			>> m_texture_fps
			>> (m_is_anim_texture_cycled)
			>> (m_rate)
			>> (m_PResistance)
			>> (m_PSpin)
			>> (m_PSpinSpread)
			>> (m_life)
			>> (m_PLifeSpread)
			>> (m_size)
			>> (m_color_alpha)
			>> (m_PActingForce)
			>> (m_velocity)
			>> (m_PInitialVelSpread)
			>> (m_vel_spread_amp)
			>> m_rnd_frame
			>> m_is_play_tex_anim;

		setMaxParticles( m_nMaxParticles );
	}

}
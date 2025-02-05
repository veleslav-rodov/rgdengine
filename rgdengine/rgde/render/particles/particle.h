#pragma once

#include <rgde/math/types3d.h>

namespace particles
{
	// basic typical particle.
	// serve as a base class for more advanced particles
	struct base_particle
	{
		base_particle();

		math::vec3f pos;
		float size;					// size variation
		math::Color color;			// color variation
		float rotation;				// rotation variation
	};

	/// advanced particle struct
	struct particle : public base_particle
	{
		particle();

		float mass;
		float ttl;				// ttl(time to live) variation
		float time;
		float old_time;
		bool  dead;				// used for marking "dead" particles
		float rot_speed;
		float initial_spin;
		float cur_tex_frame;

		math::vec3f vel;
		math::vec3f vel_spread;
		math::vec3f initial_vel;
		math::vec3f initial_pos;
		math::vec3f sum_vel;
		math::vec3f old_pos;
	};

	typedef std::vector<particle> particles_t;
	typedef particles_t::iterator  particles_iter;

	inline base_particle::base_particle() 
	: size(0)
	, rotation(0)
	, color(255, 255, 255, 255)
	{
	}

	inline particle::particle() 
	: dead(true)
	, initial_spin(0)
	, rot_speed(0)
	, mass(0)
	, time(0)
	, ttl(0)
	, cur_tex_frame(0)
	{
	}
}
#pragma once

#include <engine/core/math/forward.h>
#include <engine/core/types.h>

namespace rgde
{
	namespace math
	{
		struct color
		{
			color(const vec3f& v);
			color(const vec4f& v);

			color(unsigned long c = 0);
			color(const color& c);
			color(uchar r, uchar g, uchar b, uchar a);

			void set(uchar r, uchar g, uchar b, uchar a);
			void set(float r, float g, float b, float a);

			color& operator=(const vec4f& vec);

			operator vec4f() const;
			inline operator unsigned int() const {return data;}

			union
			{
				unsigned int data;
				struct {uchar b, g, r, a;};
			};
		};

		const color DarkGreen (0,127,0,255);
		const color Green (0,255,0,255);
		const color DarkRed	 (127,0,0,255);
		const color Red	  (255,0,0,255);
		const color DarkBlue (0,0,127,255);
		const color Blue  (0,0,255,255);		
		const color White (255,255,255,255);
		const color Grey (127,127,127,255);
		const color Black (0,0,0,255);

		color operator*( const color& color, float factor );
		color operator+( const color& c1, const color& c2 );
		color operator-( const color& c1, const color& c2 );
	}
}
#pragma once

#include <rgde/core/named_object.h>
#include <rgde/math/types3d.h>
#include <rgde/render/texture.h>
#include <rgde/render/effect.h>

#include <rgde/render/binders.h>


namespace render
{
	typedef boost::shared_ptr<class material> material_ptr;

	class material : public core::named_object//, game::dynamic_object
	{
		material(math::Color amb, math::Color diff, math::Color spec,
				  math::Color em, float power);

		void load(const std::string& filename);

	public:
		class material_map
		{
		public:
			enum default_texture_type
			{
				Black            = 1,
				White            = 2,
				DefaultNormalMap = 3
			};

			material_map(default_texture_type defaultTexture = White);

			const texture_ptr& get_texture() const { return m_texture; }
			void texture(const texture_ptr& texure);

			//Returns true if texture was valid for the last call of texture
			bool isTextureValid() const { return m_bTextureIsValid; }

			const math::matrix44f& getMatrix() const { return m_matrix; }
			math::matrix44f&       getMatrix()       { return m_matrix; }

			const math::vec3f& getScrollSpeed() const { return m_scrollSpeed; }
			math::vec3f&       getScrollSpeed()       { return m_scrollSpeed; }

			float	getRotationSpeed() const			{ return m_fRotationSpeed; }
			void	setRotationSpeed(float f)			{ m_fRotationSpeed = f;}

			const math::vec3f&	getRotationCenter() const { return m_rotationCenter; }
			void				setRotationCenter(const math::vec3f& v)       {m_rotationCenter = v; }

			void update(float dt);

		private:
			texture_ptr        m_pDefaultTexture;

			bool            m_bTextureIsValid;

			texture_ptr        m_texture;
			math::matrix44f m_matrix;
			math::vec3f     m_scrollSpeed;
			float           m_fRotationSpeed;
			math::vec3f     m_rotationCenter;

			float m_time;
		};

		typedef std::map<std::string, material_map> MaterialMaps;

		static material_ptr create(math::Color amb = math::Black,
				  math::Color diff = math::White,
				  math::Color spec = math::Black,
				  math::Color em = math::Black,
				  float power = 1.0f);

		static material_ptr create(const std::string& filename);

		virtual ~material();

		bool isTransparent() const;

		const math::Color& getDiffuseColor()	const {return m_diffuse;}
		const math::Color& getAmbientColor()	const {return m_ambient;}
		const math::Color& getSpecularColor()	const {return m_specular;}
		const math::Color& getEmissiveColor()	const {return m_emissive;}

		void setDiffuseColor(const math::Color& c)	{m_diffuse  = c;}
		void setAmbientColor(const math::Color& c)	{m_ambient  = c;}
		void setSpecularColor(const math::Color& c) {m_specular = c;}
		void setEmissiveColor(const math::Color& c) {m_emissive = c;}

		float getSpecularPower()		const	{return m_fPower;}
		void setSpecularPower(float p)			{m_fPower = p;}		

		material_map&		getMaterialMap( const std::string& type);
		const material_map& getMaterialMap( const std::string& type) const;

		const texture_ptr&		getTextureMap( const std::string& type) const;

		MaterialMaps&		getMaterialMaps()		{ return m_maps;}
		const MaterialMaps& getMaterialMaps() const { return m_maps;}
		
		void update(float dt);

		void  setEffect(const effect_ptr& effect);
		const dynamic_binder_ptr& getDynamicBinder();
		effect::technique* getTechnique() const;

	protected:		
		dynamic_binder_ptr	m_pDynamicBinder;
		effect::technique* m_technique;
		MaterialMaps	m_maps;
		math::Color     m_diffuse;        // Diffuse color RGBA 
		math::Color     m_ambient;        // Ambient color RGB 
		math::Color     m_specular;       // Specular 'shininess' 
		math::Color     m_emissive;       // Emissive color RGB 
		float           m_fPower;         // Sharpness if specular highlight

		std::string		m_file_name;
	};
}
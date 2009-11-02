#include "precompiled.h"

#include <rgde/render/material.h>
#include <rgde/render/manager.h>
#include <rgde/render/binders.h>

#include <rgde/base/xml_helpers.h>

namespace render
{
	void material::material_map::setTexture(const texture_ptr& texture)
	{
		if (texture != NULL)
		{
			m_texture = texture;
			m_bTextureIsValid = true;
		}
		else
		{
			m_texture = m_pDefaultTexture;
			m_bTextureIsValid = false;
			//m_bTextureIsValid = true;
		}
	}

	void material::material_map::update(float dt)
	{
		m_time += dt;

		math::Matrix44f tempMatrix;

		math::setTrans(tempMatrix, m_scrollSpeed * m_time);
		m_matrix = tempMatrix;

		math::setTrans(tempMatrix, m_rotationCenter);
		m_matrix *= tempMatrix;

		math::setRot(tempMatrix, math::AxisAnglef(m_fRotationSpeed * m_time, math::Vec3f(0.0f, 0.0f, 1.0f)));//Is it right? (rotating around Z axis)
		m_matrix *= tempMatrix;

		math::setTrans(tempMatrix, -m_rotationCenter);
		m_matrix *= tempMatrix;
	}

	texture_ptr& getDefaultTextureByType(material::material_map::EDefaultTexture defaultTexture)
	{
		switch (defaultTexture)
		{
		case material::material_map::Black:
			return TheRenderManager::get().getBlackTexture();
		case material::material_map::White:
			return TheRenderManager::get().getWhiteTexture();
		case material::material_map::DefaultNormalMap:
			return TheRenderManager::get().getDefaultNormalMap();
		}

		static texture_ptr empti_texture_ptr;
		return empti_texture_ptr;
	}

	material::material_map::material_map(EDefaultTexture defaultTexture)
	{
		m_pDefaultTexture = getDefaultTextureByType(defaultTexture);
		texture_ptr pNullTexture;
		setTexture(pNullTexture);

		m_fRotationSpeed = 0.0f;
		m_time = 0.0;
	}

	PMaterial material::create(math::Color amb, math::Color diff, math::Color spec, math::Color em, float power)
	{
		return PMaterial(new material(amb, diff, spec, em, power));
	}

	PMaterial material::create(const std::string& file_name)
	{
		PMaterial mat = material::create();
		mat->load(file_name);
		return mat;
	}

	material::material(math::Color amb, math::Color diff, math::Color spec, math::Color em, float power)
		: m_ambient(amb),
		  m_diffuse(diff),
		  m_specular(spec),
		  m_emissive(em),
		  m_fPower(power)
	{
		m_maps["diffuse"] = material_map();
		m_maps["illumination"] = material_map(material_map::Black);
		m_maps["reflection"] = material_map(material_map::Black);
		m_maps["bump"] = material_map(material_map::DefaultNormalMap);
		m_maps["lightmap"] = material_map();
		//base::lmsg << "material::material()";
	}

	material::~material()
	{
		//base::lmsg << "material::~material()";
	}

	bool material::isTransparent() const
	{
		bool texHasAlpha	= false;
		MaterialMaps::const_iterator it = m_maps.find("diffuse");
		if(it->second.isTextureValid())
			texHasAlpha = it->second.getTexture()->has_alpha();

		if (m_diffuse.a == 255 && !texHasAlpha)
			return false;
		else
			return true;
	}

	int getMapId(TiXmlElement *elem)
	{
		int id					= -1;
		TiXmlElement *pMapElem	= elem->FirstChildElement("map");
		if (NULL != pMapElem)
			pMapElem->Attribute("id", &id);

		return id;
	}

	TiXmlElement * assignMap(TiXmlElement *elem, std::string name, std::vector<texture_ptr> &textures, material::MaterialMaps &maps)
	{
		TiXmlElement *pMapEl= elem->FirstChildElement(name);
		if (NULL == pMapEl)
			return NULL;

		int id	= getMapId(pMapEl);

		if (id >= 0)
			maps[name].setTexture(textures[id]);

		return pMapEl;
	}

	float readFloatValue(TiXmlElement *elem, std::string name, float default_value = 0.0f)
	{
		double value= default_value;

		if (NULL == elem)
			return 0;

		TiXmlElement *child	= elem->FirstChildElement(name);

		if (NULL == child)
			return 0;

		child->Attribute("value", &value);
		return (float)value;
	}

	void loadMaterialTextures(TiXmlElement *elem, std::vector<texture_ptr> &textures)
	{
		TiXmlElement *maps	= elem->FirstChildElement("maps");
		if (maps)
		{
			TiXmlNode* pNode = NULL;
			while (pNode = maps->IterateChildren(pNode))
			{
				TiXmlElement *tx = pNode->ToElement();
				if (0 != tx->Attribute("file"))
				{
					std::string str	= tx->Attribute("file");
					texture_ptr tex	= texture::create(str);
					textures.push_back(tex);
				}
				else
				{
					textures.push_back(texture_ptr());
				}
			};
		}
	}

	void material::load(const std::string& file_name)
	{
		m_file_name = file_name;
		base::lmsg << "loading material: " << "\"" << file_name << "\"";

		TiXmlDocument mat;
		{
			io::path_add_scoped p	("Materials/");
			if (!base::load_xml(file_name, mat))
			{
				base::lerr << "�� ���� ��������� ���� ���������!" << "\"" << file_name << "\"";
				return;
			}
		}

		TiXmlElement *elem	= mat.RootElement();

		std::vector<texture_ptr> textures;
		loadMaterialTextures(elem, textures);

		float a		= readFloatValue(elem, "opacity", 1.0f);
		float illum	= readFloatValue(elem, "illumination");
		
		m_emissive.r = m_emissive.g = m_emissive.b = static_cast<uchar>(illum*255);
		m_emissive.a = static_cast<uchar>(a*255);

		TiXmlElement *pAmbientEl= assignMap(elem, "ambient", textures, getMaterialMaps());
		base::read_color(m_ambient, pAmbientEl, a);

		TiXmlElement *pDiffEl	= assignMap(elem, "diffuse", textures, getMaterialMaps());
		base::read_color(m_diffuse, pDiffEl, a);

		TiXmlElement *pSpecEl	= assignMap(elem, "specular", textures, getMaterialMaps());
		base::read_color(m_specular, pSpecEl, a);

		//I didn't know how it works so I made as knew :)
		assignMap(elem, "illumination", textures, getMaterialMaps());
		assignMap(elem, "reflection", textures, getMaterialMaps());
		assignMap(elem, "bump", textures, getMaterialMaps());
		assignMap(elem, "lightmap", textures, getMaterialMaps());

		m_fPower = 255.0f;
	}

	const material::material_map & material::getMaterialMap(const std::string &type) const
	{
		MaterialMaps::const_iterator it	= m_maps.find(type);
		return it->second;
	}

	material::material_map & material::getMaterialMap(const std::string &type)
	{
		MaterialMaps::iterator it	= m_maps.find(type);
		return it->second;
	}

	const texture_ptr & material::getTextureMap(const std::string &type) const
	{
		return getMaterialMap(type).getTexture();
	}

	void material::update(float dt)
	{
		MaterialMaps::iterator it	= m_maps.begin();
		for (; it != m_maps.end(); it++)
			it->second.update(dt);
	}

	void material::setEffect(const effect_ptr& pEffect)
	{
		if(m_technique)
			m_technique = NULL;
		if(m_pDynamicBinder)
			m_pDynamicBinder = dynamic_binder_ptr();

		std::string techName;

		m_pDynamicBinder = createDynamicBinder(pEffect, *this, techName);

		m_technique = pEffect->findTechnique(techName);

		if(!m_technique)
			base::lwrn<<"Technique \""<<techName<<"\" not found in effect \""<<pEffect->get_name()<<"\".";
	}

	const dynamic_binder_ptr& material::getDynamicBinder()
	{
		if(!m_pDynamicBinder)
			setEffect(TheRenderManager::get().getDefaultEffect());
		return m_pDynamicBinder;
	}

	effect::ITechnique* material::getTechnique() const
	{
		return m_technique;
	}
}
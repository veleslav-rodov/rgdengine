#include "precompiled.h"

#include <rgde/render/renderMaterial.h>
#include <rgde/render/renderManager.h>
#include <rgde/render/renderBinders.h>

#include <rgde/base/xml_helpers.h>

namespace render
{
	void Material::MaterialMap::setTexture(const PTexture& texture)
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

	void Material::MaterialMap::update(float dt)
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

	PTexture& getDefaultTextureByType(Material::MaterialMap::EDefaultTexture defaultTexture)
	{
		switch (defaultTexture)
		{
		case Material::MaterialMap::Black:
			return TheRenderManager::Get().getBlackTexture();
		case Material::MaterialMap::White:
			return TheRenderManager::Get().getWhiteTexture();
		case Material::MaterialMap::DefaultNormalMap:
			return TheRenderManager::Get().getDefaultNormalMap();
		}

		static PTexture empti_texture_ptr;
		return empti_texture_ptr;
	}

	Material::MaterialMap::MaterialMap(EDefaultTexture defaultTexture)
	{
		m_pDefaultTexture = getDefaultTextureByType(defaultTexture);
		PTexture pNullTexture;
		setTexture(pNullTexture);

		m_fRotationSpeed = 0.0f;
		m_time = 0.0;
	}

	PMaterial Material::Create(math::Color amb, math::Color diff, math::Color spec, math::Color em, float power)
	{
		return PMaterial(new Material(amb, diff, spec, em, power));
	}

	PMaterial Material::Create(const std::string& fileName)
	{
		PMaterial mat = Material::Create();
		mat->load(fileName);
		return mat;
	}

	Material::Material(math::Color amb, math::Color diff, math::Color spec, math::Color em, float power)
		: m_ambient(amb),
		  m_diffuse(diff),
		  m_specular(spec),
		  m_emissive(em),
		  m_fPower(power)
	{
		m_maps["diffuse"] = MaterialMap();
		m_maps["illumination"] = MaterialMap(MaterialMap::Black);
		m_maps["reflection"] = MaterialMap(MaterialMap::Black);
		m_maps["bump"] = MaterialMap(MaterialMap::DefaultNormalMap);
		m_maps["lightmap"] = MaterialMap();
		//base::lmsg << "Material::Material()";
	}

	Material::~Material()
	{
		//base::lmsg << "Material::~Material()";
	}

	bool Material::isTransparent() const
	{
		bool texHasAlpha	= false;
		MaterialMaps::const_iterator it = m_maps.find("diffuse");
		if(it->second.isTextureValid())
			texHasAlpha = it->second.getTexture()->hasAlpha();

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

	TiXmlElement * assignMap(TiXmlElement *elem, std::string name, std::vector<PTexture> &textures, Material::MaterialMaps &maps)
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

	void loadMaterialTextures(TiXmlElement *elem, std::vector<PTexture> &textures)
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
					PTexture tex	= ITexture::Create(str);
					textures.push_back(tex);
				}
				else
				{
					textures.push_back(PTexture());
				}
			};
		}
	}

	void Material::load(const std::string& fileName)
	{
		m_file_name = fileName;
		base::lmsg << "loading material: " << "\"" << fileName << "\"";

		TiXmlDocument mat;
		{
			io::ScopePathAdd p	("Materials/");
			if (!base::loadXml(fileName, mat))
			{
				base::lerr << "�� ���� ��������� ���� ���������!" << "\"" << fileName << "\"";
				return;
			}
		}

		TiXmlElement *elem	= mat.RootElement();

		std::vector<PTexture> textures;
		loadMaterialTextures(elem, textures);

		float a		= readFloatValue(elem, "opacity", 1.0f);
		float illum	= readFloatValue(elem, "illumination");
		
		m_emissive.r = m_emissive.g = m_emissive.b = static_cast<uchar>(illum*255);
		m_emissive.a = static_cast<uchar>(a*255);

		TiXmlElement *pAmbientEl= assignMap(elem, "ambient", textures, getMaterialMaps());
		base::readColorFromXml(m_ambient, pAmbientEl, a);

		TiXmlElement *pDiffEl	= assignMap(elem, "diffuse", textures, getMaterialMaps());
		base::readColorFromXml(m_diffuse, pDiffEl, a);

		TiXmlElement *pSpecEl	= assignMap(elem, "specular", textures, getMaterialMaps());
		base::readColorFromXml(m_specular, pSpecEl, a);

		//I didn't know how it works so I made as knew :)
		assignMap(elem, "illumination", textures, getMaterialMaps());
		assignMap(elem, "reflection", textures, getMaterialMaps());
		assignMap(elem, "bump", textures, getMaterialMaps());
		assignMap(elem, "lightmap", textures, getMaterialMaps());

		m_fPower = 255.0f;
	}

	const Material::MaterialMap & Material::getMaterialMap(const std::string &type) const
	{
		MaterialMaps::const_iterator it	= m_maps.find(type);
		return it->second;
	}

	Material::MaterialMap & Material::getMaterialMap(const std::string &type)
	{
		MaterialMaps::iterator it	= m_maps.find(type);
		return it->second;
	}

	const PTexture & Material::getTextureMap(const std::string &type) const
	{
		return getMaterialMap(type).getTexture();
	}

	void Material::update(float dt)
	{
		MaterialMaps::iterator it	= m_maps.begin();
		for (; it != m_maps.end(); it++)
			it->second.update(dt);
	}

	void Material::setEffect(const PEffect& pEffect)
	{
		if(m_technique)
			m_technique = NULL;
		if(m_pDynamicBinder)
			m_pDynamicBinder = PDynamicBinder();

		std::string techName;

		m_pDynamicBinder = createDynamicBinder(pEffect, *this, techName);

		m_technique = pEffect->findTechnique(techName);

		if(!m_technique)
			base::lwrn<<"Technique \""<<techName<<"\" not found in effect \""<<pEffect->getName()<<"\".";
	}

	const PDynamicBinder& Material::getDynamicBinder()
	{
		if(!m_pDynamicBinder)
			setEffect(TheRenderManager::Get().getDefaultEffect());
		return m_pDynamicBinder;
	}

	Effect::ITechnique* Material::getTechnique() const
	{
		return m_technique;
	}
}
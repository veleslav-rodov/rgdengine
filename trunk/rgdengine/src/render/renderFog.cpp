#include "precompiled.h"

#include <rgde/render/renderFog.h>

#include <rgde/base/xml_helpers.h>
#include <rgde/base/log.h>
#include <rgde/base/lexical_cast.h>

namespace render
{
	inline Fog::Type convertMode(const std::string &strMode)
	{
		std::string strModeLow	= strMode;

		base::lower_case<std::string>(strModeLow);

		if (strModeLow == "exp")
			return Fog::Exp;
		else if (strModeLow == "exp2")
			return Fog::Exp2;
		else if (strModeLow == "linear")
			return Fog::Linear;
		else
			return Fog::None;
	}

	void Fog::loadFromXML(TiXmlElement *node)
	{
		base::read(m_color, node, "color");

		std::string strMode	= base::safeReadValue<std::string>(node, "mode", "None");
		setMode(convertMode(strMode));

		m_fStart = base::safeReadValue<float>(node, "start", 0.0f);
		m_fEnd = base::safeReadValue<float>(node, "end", 0.0f);
	}

	void Fog::loadFromXML(const std::string &strFileName)
	{
		TiXmlDocument fog;
		{
			io::ScopePathAdd p	("Fogs/");
			if (!base::loadXml(strFileName, fog))
			{
				base::lerr << "Can't load fog \"" << strFileName << "\".";
				return;
			}
		}

		TiXmlElement *root	= fog.FirstChildElement("fog");

		if (0 != root)
			loadFromXML(root);
	}

	void Fog::setMode(Type type)
	{
		m_type = type;
		m_bEnabled = m_type != None;
	}

	Fog::Fog()
		: m_fStart(0),
		  m_fEnd(0)
	{
		setMode(None);
	}
}
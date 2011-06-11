#pragma once

#include "editbox.h"
#include "button.h"
#include "list.h"

#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif

namespace gui
{

class  Combobox :
	public Editbox
{
public:
	typedef Combobox Self;
	explicit Combobox(System& sys, const std::string& name = std::string());
	virtual ~Combobox();

	static const char* GetType() { return "Combobox"; }
	virtual const char* getType() { return Self::GetType(); }

	virtual bool onLoad();
	virtual bool onMouseButton(EventArgs::MouseButtons btn, EventArgs::ButtonState state);
	virtual bool onFocusLost(BaseWindow* newFocus);
	void onBtnClick(const events::ClickEvent& e);
	void onListClick(const events::ClickEvent& e);

	void AddItem(const std::string& name);

protected:
	WindowPtr m_btn;
	WindowPtr m_list;

private:
	void hideList()
	{
		if (m_list && m_list->visible())
		{
			addChildWindow(m_list.get());
			m_list->visible(false);
		}
	}

};

}

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

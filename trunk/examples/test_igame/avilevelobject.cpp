//AviLevelObject.cpp
#include "AviLevelObject.h"
#include <iostream>

AviLevelObject::AviLevelObject()
{
	std::cout << "1: AviLevelObject created. (press SPACE for send events::on_complite_level)\n";

    {
        using namespace input;

        m_cSpace.attach(L"Space");
		Input::get_device(types::Keyboard)->get_control(types::KeySpace )->bind(L"Space");        
        m_cSpace += boost::bind(&AviLevelObject::onSpace, this);
    }
}

AviLevelObject::~AviLevelObject()
{
	std::cout << "AviLevelObject destroyed." << std::endl;
}

void AviLevelObject::onSpace() 
{
	send_event<game::events::on_complite_level>(game::events::on_complite_level());
}

namespace game
{
	REGISTER_LEVEL_TYPE(AviLevelObject)
}
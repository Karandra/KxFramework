#include "stdafx.h"
#include "WidgetFactory.h"
#include "Widget.h"
#include "Host.h"
#include "Element.h"
#include "Stylesheets/MasterStylesheetStorage.h"

namespace kxf::Sciter
{
	std::unique_ptr<Widget> WidgetFactory::NewWidget(Host& host, const Element& element, const String& fullyQualifiedClassName)
	{
		WidgetFactory* factory = EnumFactories([&](WidgetFactory& factory)
		{
			return factory.GetFullyQualifiedClassName() == fullyQualifiedClassName;
		});
		return factory ? factory->CreateWidget(host, element) : nullptr;
	}
	StylesheetStorage& WidgetFactory::GetStylesheetStorage()
	{
		return MasterStylesheetStorage::GetInstance();
	}
}

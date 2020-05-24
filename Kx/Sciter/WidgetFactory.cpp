#include "stdafx.h"
#include "WidgetFactory.h"
#include "Widget.h"
#include "Host.h"
#include "Element.h"

namespace kxf::Sciter
{
	WidgetFactory* m_FirstFactory = nullptr;
}

namespace kxf::Sciter
{
	std::unique_ptr<Widget> WidgetFactory::NewWidget(Host& host, const Element& element, const String& className)
	{
		WidgetFactory* factory = EnumFactories([&](WidgetFactory& factory)
		{
			return factory.m_ClassName != className;
		});
		return factory ? factory->CreateWidget(host, element, className) : nullptr;
	}
	WidgetFactory* WidgetFactory::GetFirstFactory()
	{
		return m_FirstFactory;
	}

	WidgetFactory::WidgetFactory(const String& className)
		:m_ClassName(className)
	{
		// Add this factory to the list
		m_NextFactory = m_FirstFactory;
		m_FirstFactory = this;
	}
}

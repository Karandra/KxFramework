#include "stdafx.h"
#include "NativeTextBoxWidget.h"

namespace kxf::Sciter::Private
{
	void RegisterFactories()
	{
		NativeTextBoxWidgetFactory::RegisterInstance();
	}
}

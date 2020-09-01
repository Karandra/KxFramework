#include "stdafx.h"
#include "NativeTextBoxWidget.h"

namespace
{
	const constexpr std::wstring_view g_Stylesheet =
		#include "NativeTextBoxWidget.css"
		;
}

namespace kxf::Sciter
{
	String NativeTextBoxWidgetFactory::GetWidgetStylesheet() const
	{
		return String::FromView(g_Stylesheet);
	}
}

#include "KxfPCH.h"
#include "INativeWidget.h"
#include "IWidget.h"
#include "kxf/Application/IGUIApplication.h"

namespace kxf::Widgets
{
	void* GetNativeHandleOf(const IWidget& widget) noexcept
	{
		if (auto nativeWidget = widget.QueryInterface<INativeWidget>())
		{
			return nativeWidget->GetNativeHandle();
		}
		return nullptr;
	}
	void* GetShellNativeHandle() noexcept
	{
		return ::GetShellWindow();
	}
	void* GetAnyTopLevelNativeHandle(const IWidget* widget) noexcept
	{
		if (auto window = widget ? ::wxGetTopLevelParent(widget->GetWxWindow()) : nullptr)
		{
			return window->GetHandle();
		}
		else if (auto app = IGUIApplication::GetInstance(); window = app ? app->GetTopWindow() : nullptr)
		{
			return window->GetHandle();
		}
		else if (window = ::wxGetTopLevelParent(::wxGetActiveWindow()))
		{
			return window->GetHandle();
		}
		return GetShellNativeHandle();
	}
}

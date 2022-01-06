#include "KxfPCH.h"
#include "INativeWidget.h"
#include "IWidget.h"
#include "kxf/EventSystem/IEvtHandler.h"
#include "kxf/Application/IGUIApplication.h"
#include "kxf/UI/IWidget.h"
#include "kxf/UI/ITopLevelWidget.h"

namespace kxf
{
	void INativeWidget::OnMessageProcessed(void* handle, uint32_t messageID, intptr_t result)
	{
		QueryInterface<IEvtHandler>()->ProcessSignal(&INativeWidget::OnMessageProcessed, handle, messageID, result);
	}
}

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
		wxWindow* window = nullptr;
		if (window = widget ? ::wxGetTopLevelParent(widget->GetWxWindow()) : nullptr)
		{
			return window->GetHandle();
		}
		else if (auto app = IGUIApplication::GetInstance())
		{
			if (auto widget = app->GetTopWidget())
			{
				window = widget->GetWxWindow();
			}
		}

		if (window = window ? window : ::wxGetTopLevelParent(::wxGetActiveWindow()))
		{
			return window->GetHandle();
		}
		return GetShellNativeHandle();
	}
}

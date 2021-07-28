#include "KxfPCH.h"
#include "WidgetTextEvent.h"
#include "kxf/Network/URI.h"

namespace kxf
{
	WidgetTextEvent::WidgetTextEvent(IWidget& widget, const URI& uri, WidgetMouseEvent mouseEvent) noexcept
		:WidgetEvent(widget), m_MouseEvent(std::move(mouseEvent))
	{
		WidgetEvent::SetString(uri.BuildURI());
	}

	// WidgetURIEvent
	URI WidgetTextEvent::GetURI() const
	{
		return WidgetEvent::GetString();
	}
}

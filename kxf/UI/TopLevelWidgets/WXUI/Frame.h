#pragma once
#include "Common.h"
#include "kxf/UI/ITopLevelWidget.h"
#include <wx/frame.h>

namespace kxf::WXUI
{
	class KX_API Frame: public EvtHandlerWrapper<wxFrame>
	{
		private:
			ITopLevelWidget& m_Widget;

		public:
			Frame(ITopLevelWidget& widget)
				:EvtHandlerWrapper(widget), m_Widget(widget)
			{
			}

		public:
			bool Create(wxWindow* parent,
						const String& title,
						const Point& pos = Point::UnspecifiedPosition(),
						const Size& size = Size::UnspecifiedSize()
			);

		public:
			// wxTopLevelWindow
			bool ShouldPreventAppExit() const override
			{
				return m_Widget.ShouldPreventApplicationExit();
			}
	};
}

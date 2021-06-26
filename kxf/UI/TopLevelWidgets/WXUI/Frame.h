#pragma once
#include "kxf/UI/IWidget.h"
#include <wx/frame.h>

namespace kxf::WXUI
{
	class KX_API Frame: public wxFrame
	{
		private:
			IWidget& m_Widget;

		public:
			Frame(IWidget& widget)
				:m_Widget(widget)
			{
			}
			bool Create(wxWindow* parent,
						const String& title,
						const Point& pos = Point::UnspecifiedPosition(),
						const Size& size = Size::UnspecifiedSize()
			);
	};
}

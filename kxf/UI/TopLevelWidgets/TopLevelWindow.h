#pragma once
#include "Common.h"

namespace kxf::WXUI
{
	class Frame;
}

namespace kxf::Widgets
{
	class KX_API TopLevelWindow: public RTTI::Implementation<TopLevelWindow, Private::BasicTLWindowWidget<TopLevelWindow, WXUI::Frame, ITopLevelWidget>>
	{
		public:
			TopLevelWindow();
			~TopLevelWindow();

		public:
			// IWidget
			bool CreateWidget(IWidget* parent, const String& text, Point pos = Point::UnspecifiedPosition(), Size size = Size::UnspecifiedSize()) override;
	};
}

#pragma once
#include "Common.h"
#include "kxf/UI/Private/BasicWindowWidget.h"

namespace kxf::WXUI
{
	class Frame;
}

namespace kxf::Widgets
{
	class KX_API Window: public RTTI::Implementation<Window, Private::BasicWindowWidget<Window, WXUI::Frame, ITopLevelWidget>>
	{
		public:
			Window();
			~Window();

		public:
			// IWidget
			bool CreateWidget(std::shared_ptr<IWidget> parent, const String& text = {}, Point pos = Point::UnspecifiedPosition(), Size size = Size::UnspecifiedSize()) override;
	};
}

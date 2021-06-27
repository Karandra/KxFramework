#pragma once
#include "Common.h"
#include "kxf/UI/Private/BasicDialogWidget.h"

namespace kxf::WXUI
{
	class Dialog;
}

namespace kxf::Widgets
{
	class KX_API Dialog: public RTTI::Implementation<Dialog, Private::BasicWindowWidget<Dialog, WXUI::Dialog, IDalogWidget>>
	{
		public:
			Dialog();
			~Dialog();

		public:
			// IWidget
			bool CreateWidget(IWidget* parent, const String& text, Point pos = Point::UnspecifiedPosition(), Size size = Size::UnspecifiedSize()) override;
	};
}

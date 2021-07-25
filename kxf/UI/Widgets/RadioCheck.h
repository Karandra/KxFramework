#pragma once
#include "Common.h"
#include "../ICheckWidget.h"

namespace kxf::WXUI
{
	class RadioCheck;
}

namespace kxf::Widgets
{
	class KX_API RadioCheck: public RTTI::Implementation<RadioCheck, Private::BasicWxWidget<RadioCheck, WXUI::RadioCheck, ICheckWidget>>
	{
		public:
			RadioCheck();
			~RadioCheck();

		public:
			// IWidget
			bool CreateWidget(std::shared_ptr<IWidget> parent, const String& text = {}, Point pos = Point::UnspecifiedPosition(), Size size = Size::UnspecifiedSize()) override;

			// ICheckWidget
			bool IsThirdStateSelectable() const override;
			void SetThirdStateSelectable(bool isSelectable = true) override;

			CheckWidgetValue GetValue() const override;
			void SetValue(CheckWidgetValue value) override;
	};
}

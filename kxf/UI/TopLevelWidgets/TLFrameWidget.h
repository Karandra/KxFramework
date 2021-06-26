#pragma once
#include "Common.h"

namespace kxf::WXUI
{
	class Frame;
}

namespace kxf
{
	class KX_API TLFrameWidget: public Private::BasicWidget<TLFrameWidget, WXUI::Frame, ITopLevelWidget>
	{
		friend class BasicWidget;

		private:
			void OnWindowCreate()
			{
			}
			void OnWindowDestroy()
			{
			}

		public:
			TLFrameWidget();
			~TLFrameWidget();

		public:
			// IWidget
			bool CreateWidget(IWidget* parent, const String& text, Point pos = Point::UnspecifiedPosition(), Size size = Size::UnspecifiedSize()) override;
	};
}

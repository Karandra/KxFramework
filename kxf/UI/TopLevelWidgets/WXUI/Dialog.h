#pragma once
#include "Common.h"
#include "kxf/UI/IDalogWidget.h"
#include <wx/dialog.h>

namespace kxf::WXUI
{
	class KX_API Dialog: public EvtHandlerWrapper<wxDialog>
	{
		private:
			IDalogWidget& m_Widget;

		public:
			Dialog(IDalogWidget& widget)
				:EvtHandlerWrapper(widget), m_Widget(widget)
			{
				m_Widget.SetPreventApplicationExit(false);
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

#pragma once
#include "Common.h"
#include "../../ITextWidget.h"
#include "kxf/UI/WindowRefreshScheduler.h"
#include <wx/textctrl.h>

namespace kxf::WXUI
{
	class KX_API TextBox: public EvtHandlerWrapper<UI::WindowRefreshScheduler<wxTextCtrl>>
	{
		private:
			ITextWidget& m_Widget;

		private:
			bool DoTryBefore(wxEvent& event);

		protected:
			// wxEvtHandler
			bool TryBefore(wxEvent& event) override
			{
				if (DoTryBefore(event))
				{
					return true;
				}
				return EvtHandlerWrapper::TryBefore(event);
			}

			// wxWindow
			WXLRESULT MSWWindowProc(WXUINT msg, WXWPARAM wParam, WXLPARAM lParam) override;

		public:
			TextBox(ITextWidget& widget)
				:EvtHandlerWrapper(widget), m_Widget(widget)
			{
			}

		public:
			bool Create(wxWindow* parent,
						const String& text,
						const Point& pos = Point::UnspecifiedPosition(),
						const Size& size = Size::UnspecifiedSize()
			);
	};
}

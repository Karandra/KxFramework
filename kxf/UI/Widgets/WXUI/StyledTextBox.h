#pragma once
#include "Common.h"
#include "../../IStyledTextWidget.h"
#include "kxf/UI/WindowRefreshScheduler.h"
#include <wx/stc/stc.h>

namespace kxf::WXUI
{
	class KX_API StyledTextBox: public EvtHandlerWrapper<UI::WindowRefreshScheduler<wxStyledTextCtrl>>
	{
		private:
			IStyledTextWidget& m_Widget;
			bool m_IsCreated = false;

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
		
		public:
			StyledTextBox(IStyledTextWidget& widget)
				:EvtHandlerWrapper(widget), m_Widget(widget)
			{
			}

		public:
			bool Create(wxWindow* parent,
						const String& text,
						const Point& pos = Point::UnspecifiedPosition(),
						const Size& size = Size::UnspecifiedSize()
			);
	
		public:
			// wxWindow
			bool SetBackgroundColour(const wxColour& color) override;
			bool SetForegroundColour(const wxColour& color) override;

			// StyledTextBox
			bool IsModified() const
			{
				return wxStyledTextCtrl::IsModified();
			}
			void SetModified(bool modified = true)
			{
				if (modified)
				{
					wxStyledTextCtrl::MarkDirty();
				}
				else
				{
					wxStyledTextCtrl::MarkDirty();
				}
			}
	};
}

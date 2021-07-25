#pragma once
#include "Common.h"
#include "../../ICheckWidget.h"
#include <wx/radiobut.h>

namespace kxf::WXUI
{
	class KX_API RadioCheck: public EvtHandlerWrapper<wxRadioButton>
	{
		private:
			ICheckWidget& m_Widget;

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
			RadioCheck(ICheckWidget& widget)
				:EvtHandlerWrapper(widget), m_Widget(widget)
			{
			}

		public:
			bool Create(wxWindow* parent,
						const String& label,
						const Point& pos = Point::UnspecifiedPosition(),
						const Size& size = Size::UnspecifiedSize()
			);
	};
}

#pragma once
#include "Common.h"
#include "../../IImageComboBoxWidget.h"
#include "kxf/UI/WindowRefreshScheduler.h"
#include <wx/systhemectrl.h>
#include <wx/bmpcbox.h>

namespace kxf::WXUI
{
	class KX_API ImageComboBox: public EvtHandlerWrapper<UI::WindowRefreshScheduler<wxSystemThemedControl<wxBitmapComboBox>>>
	{
		private:
			IImageComboBoxWidget& m_Widget;

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
			ImageComboBox(IImageComboBoxWidget& widget)
				:EvtHandlerWrapper(widget), m_Widget(widget)
			{
			}

		public:
			bool Create(wxWindow* parent,
						const String& label,
						const Point& pos = Point::UnspecifiedPosition(),
						const Size& size = Size::UnspecifiedSize()
			);

		public:
			bool SetVisibleItemCount(size_t count);
	};
}

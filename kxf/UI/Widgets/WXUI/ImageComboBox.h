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

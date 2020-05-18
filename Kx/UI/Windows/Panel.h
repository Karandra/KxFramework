#pragma once
#include "Kx/UI/Common.h"
#include <wx/panel.h>
#include <wx/systhemectrl.h>

namespace KxFramework::UI
{
	class KX_API Panel: public wxSystemThemedControl<wxPanel>
	{
		public:
			static constexpr WindowStyle DefaultStyle = EnumClass::Combine<WindowStyle>(WindowStyle::TabTraversal, WindowBorder::None);

		public:
			Panel() = default;
			Panel(wxWindow* parent,
				  wxWindowID id,
				  WindowStyle style = DefaultStyle
			)
			{
				Create(parent, id, style);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						WindowStyle style = DefaultStyle
			);
			bool Create(wxWindow* parent,
						wxWindowID id,
						const wxPoint& pos,
						const wxSize& size,
						WindowStyle style = DefaultStyle,
						const String& name = {}
			)
			{
				return Create(parent, id, style);
			}

		public:
			bool ShouldInheritColours() const override
			{
				return true;
			}
			bool SetBackgroundColour(const wxColour& color) override
			{
				return wxPanel::SetBackgroundColour(color);
			}
			bool SetForegroundColour(const wxColour& color) override
			{
				return wxPanel::SetForegroundColour(color);
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(Panel);
	};

	using ScrolledPanel = wxScrolled<Panel>;
}

#pragma once
#include "Kx/Sciter/Windows/Host.h"
#include <wx/control.h>

namespace KxSciter
{
	class KX_API Label: public Window<wxControl>
	{
		public:
			Label() = default;
			Label(wxWindow* parent,
				  wxWindowID id,
				  const wxString& label,
				  const wxPoint& pos = wxDefaultPosition,
				  const wxSize& size = wxDefaultSize,
				  long style = wxBORDER_NONE)
			{
				Create(parent, id, label, pos, size, style);
			}
			Label(const Label&) = delete;

			bool Create(wxWindow* parent,
						wxWindowID id,
						const wxString& label,
						const wxPoint& pos = wxDefaultPosition,
						const wxSize& size = wxDefaultSize,
						long style = wxBORDER_NONE)
			{
				if (Window::Create(parent, id, pos, size, style))
				{
					SetLabel(label);
					return true;
				}
				return false;
			}

		public:
			Label& operator=(const Label&) = delete;
	};
}

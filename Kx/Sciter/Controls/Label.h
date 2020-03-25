#pragma once
#include "Kx/Sciter/WrapperWindow.h"
#include <wx/control.h>

namespace KxFramework::Sciter
{
	class KX_API Label: public WindowWrapper<wxControl>
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
				if (WindowWrapper::Create(parent, id, pos, size, style))
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

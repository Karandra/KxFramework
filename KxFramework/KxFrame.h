#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxTopLevelWindow.h"

class KxFrame: public KxTopLevelWindow<wxFrame>
{
	public:
		static const long DefaultStyle = wxDEFAULT_FRAME_STYLE|wxFRAME_SHAPED;

		KxFrame() {}
		KxFrame(wxWindow* parent,
				wxWindowID id,
				const wxString& title,
				const wxPoint& pos = wxDefaultPosition,
				const wxSize& size = wxDefaultSize,
				long style = DefaultStyle
		)
		{
			Create(parent, id, title, pos, size, style);
		}
		bool Create(wxWindow* parent,
					wxWindowID id,
					const wxString& title,
					const wxPoint& pos = wxDefaultPosition,
					const wxSize& size = wxDefaultSize,
					long style = DefaultStyle
		);
		virtual ~KxFrame();
		
	public:
		wxDECLARE_DYNAMIC_CLASS(KxFrame);
};

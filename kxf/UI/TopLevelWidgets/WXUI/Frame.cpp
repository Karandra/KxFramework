#include "KxfPCH.h"
#include "Frame.h"

namespace kxf::WXUI
{
	bool Frame::Create(wxWindow* parent,
					   const String& title,
					   const Point& pos,
					   const Size& size
	)
	{
		if (wxFrame::Create(parent, wxID_NONE, title, pos, size, wxDEFAULT_FRAME_STYLE))
		{
			SetStatusBarPane(-1);
			Center(wxBOTH);

			return true;
		}
		return false;
	}
}

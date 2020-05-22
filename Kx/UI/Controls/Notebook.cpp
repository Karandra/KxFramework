#include "stdafx.h"
#include "Notebook.h"

namespace KxFramework::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(Notebook, wxNotebook);

	bool Notebook::Create(wxWindow* parent,
						  wxWindowID id,
						  NotebookStyle style
	)
	{
		if (wxNotebook::Create(parent, id, Point::UnspecifiedPosition(), Size::UnspecifiedSize(), ToInt(style)))
		{
			return true;
		}
		return false;
	}
}

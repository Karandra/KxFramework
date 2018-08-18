#include "KxStdAfx.h"
#include "KxFramework/KxStatusBar.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxStatusBar, wxStatusBar);

void KxStatusBar::SetFieldsCount(int count, const int* widths)
{
	if (widths == NULL)
	{
		count = GetFieldsCount();
		if (count > 0)
		{
			std::vector<int> newWidths;
			newWidths.reserve(count);
			for (int i = 0; i < count; i++)
			{
				int value = GetStatusWidth(i);
				newWidths.insert(newWidths.begin() + i, value);
			}
			wxStatusBar::SetStatusWidths(count, newWidths.data());
			return;
		}
	}
	wxStatusBar::SetFieldsCount(count, widths);
}

bool KxStatusBar::Create(wxWindow* parent,
						 wxWindowID winid,
						 int fieldsCount,
						 long style
)
{
	if (wxStatusBar::Create(parent, winid, style))
	{
		EnableSystemTheme();
		SetDoubleBuffered(true);
		SetMinHeight(DefaultHeight);

		return true;
	}
	return false;
}

void KxStatusBar::SetStatusWidth(int width, int index)
{
	int count = GetFieldsCount();
	if (count > 0 && index < count)
	{
		KxIntVector widths(count, 0);
		for (int i = 0; i < count; i++)
		{
			widths[i] = GetStatusWidth(i);
		}
		widths[index] = width;
		wxStatusBar::SetStatusWidths(count, widths.data());
	}
}

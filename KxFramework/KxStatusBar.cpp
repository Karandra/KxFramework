#include "KxStdAfx.h"
#include "KxFramework/KxStatusBar.h"

namespace
{
	constexpr int DefaultMinHeight = 23;
}

wxIMPLEMENT_DYNAMIC_CLASS(KxStatusBar, wxStatusBar);

void KxStatusBar::SetFieldsCount(int count, const int* widths)
{
	if (!widths)
	{
		if (count = GetFieldsCount(); count > 0)
		{
			std::vector<int> newWidths;
			newWidths.resize(count);
			for (size_t i = 0; i < count; i++)
			{
				newWidths[i] = GetStatusWidth(i);
			}

			wxStatusBar::SetStatusWidths(count, newWidths.data());
			return;
		}
	}
	wxStatusBar::SetFieldsCount(count, widths);
}

bool KxStatusBar::Create(wxWindow* parent,
						 wxWindowID id,
						 int fieldCount,
						 long style
)
{
	if (wxStatusBar::Create(parent, id, style))
	{
		EnableSystemTheme();
		SetDoubleBuffered(true);
		SetMinHeight(FromDIP(DefaultMinHeight));

		return true;
	}
	return false;
}

void KxStatusBar::SetStatusWidth(int width, int index)
{
	int count = GetFieldsCount();
	if (count > 0 && index < count)
	{
		std::vector<int> widths(count, 0);
		for (int i = 0; i < count; i++)
		{
			widths[i] = GetStatusWidth(i);
		}
		widths[index] = width;

		wxStatusBar::SetStatusWidths(count, widths.data());
	}
}

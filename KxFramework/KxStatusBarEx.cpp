#include "KxStdAfx.h"
#include "KxFramework/KxStatusBarEx.h"
#include "KxFramework/KxUtility.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxStatusBarEx, KxStatusBar);

void KxStatusBarEx::OnPaint(wxPaintEvent& event)
{
	wxAutoBufferedPaintDC dc(this);
	dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
	dc.SetTextForeground(GetForegroundColour());
	wxSize size = GetSize();
	KxUtility::DrawParentBackground(this, dc, size);

	// Draw background
	dc.SetBrush(GetBackgroundColour());
	dc.DrawRectangle(wxRect(wxPoint(-1, -1), size).Inflate(2));

	// Draw edge line
	if (m_ColorBorder.IsOk())
	{
		dc.SetPen(wxPen(m_ColorBorder, 1));
		dc.DrawLine(0, 0, size.GetWidth()+1, 0);
	}

	// Draw size grip
	int sizeGripWidth = 0;
	wxTopLevelWindow* topLevelWindow = dynamic_cast<wxTopLevelWindow*>(GetParent());
	if (topLevelWindow && !topLevelWindow->IsMaximized() && topLevelWindow->HasFlag(wxRESIZE_BORDER))
	{
		wxPoint gripPos(size.GetWidth() - m_SizeGripBitmap.GetWidth() - 1, GetMinHeight() - m_SizeGripBitmap.GetHeight() - 1);
		dc.DrawBitmap(m_SizeGripBitmap, gripPos, true);
		sizeGripWidth = m_SizeGripBitmap.GetWidth();
	}

	// Draw text
	int spacer = 3;
	int splitterX = 0;
	wxRect rect(spacer, 0, 0, GetMinHeight());

	for (int i = 0; i < GetFieldsCount(); i++)
	{
		GetFieldRect(i, rect);
		rect.SetX(rect.GetX() + spacer);

		wxString label;
		if (GetStatusWidth(i) != 0)
		{
			label = GetStatusText(i);
		}
		if (!label.IsEmpty())
		{
			// Ellipsize label
			int maxWidth = rect.GetWidth() - 2*GetCharWidth();
			if (i == GetFieldsCount()-1)
			{
				maxWidth -= sizeGripWidth;
			}

			// If icons enabled
			int imageIndex = -1;
			if (m_Images.count(i))
			{
				imageIndex = m_Images[i];
			}
			if (imageIndex != -1 && HasImageList())
			{
				maxWidth -= GetImageList()->GetSize().GetWidth();
			}

			// Draw label
			label = wxControl::Ellipsize(label, dc, GetEllipsizeMode(), maxWidth);
			if (imageIndex == -1 || !HasImageList())
			{
				dc.DrawLabel(label, rect, wxALIGN_CENTER_VERTICAL);
			}
			else
			{
				dc.DrawLabel(label, GetImageList()->GetBitmap(imageIndex), rect, wxALIGN_CENTER_VERTICAL);
			}
		}

		if (m_IsSeparatorsVisible && m_ColorBorder.IsOk() && i != GetFieldsCount()-1)
		{
			splitterX += rect.GetWidth();
			dc.SetPen(m_ColorBorder);
			dc.DrawLine(wxPoint(splitterX, spacer), wxPoint(splitterX, GetMinHeight() - spacer));
		}
	}
}

wxEllipsizeMode KxStatusBarEx::GetEllipsizeMode() const
{
	if (m_Style & wxSTB_ELLIPSIZE_START)
	{
		return wxEllipsizeMode::wxELLIPSIZE_START;
	}
	else if (m_Style & wxSTB_ELLIPSIZE_MIDDLE)
	{
		return wxEllipsizeMode::wxELLIPSIZE_MIDDLE;
	}
	else if (m_Style & wxSTB_ELLIPSIZE_END)
	{
		return wxEllipsizeMode::wxELLIPSIZE_END;
	}
	else
	{
		return wxEllipsizeMode::wxELLIPSIZE_NONE;
	}
}
void KxStatusBarEx::MakeTopmost()
{
	::SetWindowPos(GetHandle(), HWND_TOPMOST, 0, 0, 0, 0, SWP_ASYNCWINDOWPOS|SWP_NOMOVE|SWP_NOSIZE|SWP_NOSENDCHANGING|SWP_NOREDRAW);
}

bool KxStatusBarEx::Create(wxWindow* pParent,
						   wxWindowID id,
						   int fieldsCount,
						   long style
)
{
	SetBackgroundStyle(wxBG_STYLE_PAINT);

	if (KxStatusBar::Create(pParent, id, KxUtility::ModFlag(style, KxSBE_MASK, false)))
	{
		wxWindowUpdateLocker(this);
		
		SetSeparatorsVisible(style & KxSBE_SEPARATORS_ENABLED);
		if (style & KxSBE_INHERIT_COLORS)
		{
			SetBackgroundColour(GetParent()->GetBackgroundColour().ChangeLightness(110));
			SetForegroundColour(KxColor(GetBackgroundColour()).Negate());
			SetBorderColor(GetParent()->GetBackgroundColour().ChangeLightness(75));
		}
		else
		{
			SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
			SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
			SetBorderColor(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW));
		}

		// Configure fields
		SetFieldsCount(fieldsCount);
		SetStatusWidths(KxIntVector(fieldsCount, -1));
		for (int i = 0; i < GetFieldsCount(); i++)
		{
			m_Images[i] = NO_IMAGE;
		}

		Bind(wxEVT_PAINT, &KxStatusBarEx::OnPaint, this);
		//Bind(wxEVT_ENTER_WINDOW, &wxStatusBarEx::OnEnter, this);
		//Bind(wxEVT_LEAVE_WINDOW, &wxStatusBarEx::OnLeave, this);
		//Bind(wxEVT_LEFT_DOWN, &wxStatusBarEx::OnMouseDown, this);
		//Bind(wxEVT_LEFT_UP, &wxStatusBarEx::OnEnter, this);

		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
const char* KxStatusBarEx::m_SizeGripData[] =
{
	/* width height num_colors chars_per_pixel */
	"11 11 3 1",
	/* colors */
	"  c None",
	"! c #ffffff",
	"# c #a0a0a0",
	/* pixels */
	"        ## ",
	"        ##!",
	"         !!",
	"           ",
	"    ##  ## ",
	"    ##! ##!",
	"     !!  !!",
	"           ",
	"##  ##  ## ",
	"##! ##! ##!",
	" !!  !!  !!"
};
const wxBitmap KxStatusBarEx::m_SizeGripBitmap(m_SizeGripData);

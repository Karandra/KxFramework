#include "KxStdAfx.h"
#include "KxFramework/KxStatusBarEx.h"
#include "KxFramework/KxUtility.h"
#include "KxFramework/KxUxTheme.h"
#include "KxFramework/KxUxThemePartsAndStates.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxStatusBarEx, KxStatusBar);

void KxStatusBarEx::OnPaint(wxPaintEvent& event)
{
	wxAutoBufferedPaintDC dc(this);
	dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
	dc.SetTextForeground(GetForegroundColour());
	
	const wxSize clientSize = GetClientSize();
	KxUxTheme::DrawParentBackground(*this, dc, clientSize);

	// Draw background
	{
		dc.SetBrush(GetBackgroundColour());

		wxRect backgroundRect(wxPoint(0, 0), clientSize);
		backgroundRect.Inflate(2);
		dc.DrawRectangle(backgroundRect);

		if (m_ProgressPos > 0 && m_ProgressRange > 0)
		{
			if (KxUxTheme theme(*this, KxUxThemeClass::Progress); theme)
			{
				theme.DrawProgress(dc, -1, PP_FILL, PBFS_NORMAL, backgroundRect, m_ProgressPos, m_ProgressRange);
			}
			else
			{
				backgroundRect.SetWidth(m_ProgressPos == m_ProgressRange ? clientSize.GetWidth() : clientSize.GetWidth() * ((double)m_ProgressPos / m_ProgressRange));

				dc.SetBrush(KxColor(GetBackgroundColour()).RotateHue(30));
				dc.DrawRectangle(backgroundRect);
			}
		}
	}

	// Draw edge line
	if (m_ColorBorder.IsOk())
	{
		dc.SetPen(wxPen(m_ColorBorder, 1));
		dc.DrawLine(0, 0, clientSize.GetWidth()+1, 0);
	}

	// Draw size grip
	int sizeGripWidth = 0;
	const wxTopLevelWindow* topLevelWindow = GetTLWParent();
	if (topLevelWindow && !topLevelWindow->IsMaximized() && topLevelWindow->HasFlag(wxRESIZE_BORDER))
	{
		if (KxUxTheme theme(*this, KxUxThemeClass::Status); theme)
		{
			wxSize gripSize = theme.GetPartSize(dc, SP_GRIPPER, 0);
			wxPoint gripPos(clientSize.GetWidth() - gripSize.GetWidth() - 1, GetMinHeight() - gripSize.GetHeight() - 1);
			theme.DrawBackground(dc, SP_GRIPPER, 0, wxRect(gripPos, gripSize));
		}
		else
		{
			sizeGripWidth = clientSize.GetHeight();
		}
	}

	// Draw text
	int spacer = 3;
	int splitterX = 0;
	wxRect rect(spacer, 0, 0, GetMinHeight());

	const size_t fieldsCount = GetFieldsCount();
	for (size_t i = 0; i < fieldsCount; i++)
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
			int maxWidth = rect.GetWidth() - 2 * GetCharWidth();
			if (i == fieldsCount - 1)
			{
				maxWidth -= sizeGripWidth;
			}

			// If icons are enabled
			int imageIndex = NO_IMAGE;
			if (auto it = m_Images.find(i); it != m_Images.end())
			{
				imageIndex = it->second;
			}
			if (imageIndex != -1 && HasImageList())
			{
				maxWidth -= GetImageList()->GetSize().GetWidth();
			}

			// Draw label
			label = wxControl::Ellipsize(label, dc, GetEllipsizeMode(), maxWidth);
			if (imageIndex == NO_IMAGE || !HasImageList())
			{
				dc.DrawLabel(label, rect, wxALIGN_CENTER_VERTICAL);
			}
			else
			{
				dc.DrawLabel(label, GetImageList()->GetBitmap(imageIndex), rect, wxALIGN_CENTER_VERTICAL);
			}
		}

		if (m_IsSeparatorsVisible && m_ColorBorder.IsOk() && i != GetFieldsCount() - 1)
		{
			splitterX += rect.GetWidth();
			dc.SetPen(m_ColorBorder);
			dc.DrawLine(wxPoint(splitterX, spacer), wxPoint(splitterX, GetMinHeight() - spacer));
		}
	}
}
void KxStatusBarEx::OnSize(wxSizeEvent& event)
{
	ScheduleRefresh();
	event.Skip();
}
void KxStatusBarEx::OnMouseDown(wxMouseEvent& event)
{
	m_State = wxCONTROL_SELECTED|wxCONTROL_CURRENT|wxCONTROL_FOCUSED;

	event.Skip();
	ScheduleRefresh();
}
void KxStatusBarEx::OnEnter(wxMouseEvent& event)
{
	m_State = wxCONTROL_SELECTED|wxCONTROL_CURRENT;

	event.Skip();
	ScheduleRefresh();
}
void KxStatusBarEx::OnLeave(wxMouseEvent& event)
{
	m_State = wxCONTROL_NONE;

	event.Skip();
	ScheduleRefresh();
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
wxTopLevelWindow* KxStatusBarEx::GetTLWParent() const
{
	wxWindow* parent = GetParent();
	if (parent && parent->IsKindOf(wxCLASSINFO(wxTopLevelWindow)))
	{
		return static_cast<wxTopLevelWindow*>(parent);
	}
	return nullptr;
}

void KxStatusBarEx::MakeTopmost()
{
	::SetWindowPos(GetHandle(), HWND_TOPMOST, 0, 0, 0, 0, SWP_ASYNCWINDOWPOS|SWP_NOMOVE|SWP_NOSIZE|SWP_NOSENDCHANGING|SWP_NOREDRAW);
}

bool KxStatusBarEx::Create(wxWindow* parent,
						   wxWindowID id,
						   int fieldsCount,
						   long style
)
{
	if (KxStatusBar::Create(parent, id, KxUtility::ModFlag(style, KxSBE_MASK, false)))
	{
		EnableSystemTheme();
		SetBackgroundStyle(wxBG_STYLE_PAINT);
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
		Bind(wxEVT_SIZE, &KxStatusBarEx::OnSize, this);
		Bind(wxEVT_ENTER_WINDOW, &KxStatusBarEx::OnEnter, this);
		Bind(wxEVT_LEAVE_WINDOW, &KxStatusBarEx::OnLeave, this);
		Bind(wxEVT_LEFT_DOWN, &KxStatusBarEx::OnMouseDown, this);
		Bind(wxEVT_LEFT_UP, &KxStatusBarEx::OnEnter, this);

		return true;
	}
	return false;
}

bool KxStatusBarEx::SetForegroundColour(const wxColour& colour)
{
	ScheduleRefresh();
	return KxStatusBar::SetForegroundColour(colour);
}
bool KxStatusBarEx::SetBackgroundColour(const wxColour& color)
{
	ScheduleRefresh();

	if (color.IsOk())
	{
		return KxStatusBar::SetBackgroundColour(color);
	}
	else
	{
		wxColour newColor = GetParent()->GetBackgroundColour().ChangeLightness(110);
		return KxStatusBar::SetBackgroundColour(newColor);
	}
}

void KxStatusBarEx::SetMinHeight(int height)
{
	KxStatusBar::SetMinHeight(height);
	wxFrame* parentFrame = dynamic_cast<wxFrame*>(GetParent());
	if (parentFrame)
	{
		SetMinSize(wxSize(wxDefaultCoord, height));
		parentFrame->SetStatusBar(this);
	}
	ScheduleRefresh();
}

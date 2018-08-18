#include "KxStdAfx.h"
#include "KxFramework/KxImageList.h"
#include "KxFramework/KxButton.h"
#include "KxFramework/KxLibrary.h"
#include "KxFramework/KxMenu.h"
#include "KxFramework/KxUtility.h"
#include "KxFramework/KxIncludeWindows.h"

wxDEFINE_EVENT_ALIAS(KxEVT_BUTTON, wxCommandEvent, wxEVT_BUTTON);
wxDEFINE_EVENT(KxEVT_BUTTON_MENU, wxContextMenuEvent);

wxIMPLEMENT_DYNAMIC_CLASS(KxButton, wxButton);

wxSize KxButton::GetDefaultSize()
{
	return wxButton::GetDefaultSize();
}

void KxButton::OnPaint(wxPaintEvent& event)
{
	wxAutoBufferedPaintDC dc(this);
	KxUtility::ClearDC(this, dc);

	bool isEnabled = IsThisEnabled();
	wxSize size = GetSize();
	int width = size.GetWidth();
	int widthMod = 2;
	if (m_IsSliptterEnabled)
	{
		width -= ms_ArrowButtonWidth;
		widthMod = 5;
	}
	wxRect rect(-1, -1, width + widthMod, size.GetHeight() + 2);

	// Draw first part
	dc.SetTextForeground(isEnabled ? GetForegroundColour() : GetForegroundColour().MakeDisabled());
	wxRendererNative::Get().DrawPushButton(this, dc, rect, isEnabled ? m_ControlState : wxCONTROL_DISABLED);

	// Draw focus rectangle
	if (m_IsFocusDrawingAllowed && HasFocus())
	{
		wxRendererNative::Get().DrawFocusRect(this, dc, wxRect(2, 2, size.GetWidth() - 4, size.GetHeight() - 4), wxCONTROL_SELECTED);
	}

	// Draw bitmap and label
	rect.y += (size.GetHeight() + 2 - GetCharHeight()) / 2;
	wxBitmap bitmap = GetBitmap();
	if (bitmap.IsOk())
	{
		if (!isEnabled)
		{
			bitmap = bitmap.ConvertToDisabled();
		}
		dc.DrawLabel(GetLabelText(), bitmap, rect, wxALIGN_CENTER_HORIZONTAL);
	}
	else
	{
		dc.DrawLabel(GetLabelText(), rect, wxALIGN_CENTER_HORIZONTAL);
	}

	// Draw second part of the button
	if (m_IsSliptterEnabled)
	{
		rect.x = width+1;
		rect.y = -1;
		rect.width = ms_ArrowButtonWidth;
		rect.height = size.GetHeight() + 2;

		wxRendererNative::Get().DrawPushButton(this, dc, rect, m_ControlState);
		wxRendererNative::Get().DrawDropArrow(this, dc, rect, m_ControlState);
	}
}
void KxButton::OnMouseLeave(wxMouseEvent& event)
{
	m_ControlState = wxCONTROL_NONE;
	Refresh();
	event.Skip();
}
void KxButton::OnMouseEnter(wxMouseEvent& event)
{
	m_ControlState = wxCONTROL_CURRENT;
	Refresh();
	event.Skip();
}
void KxButton::OnLeftButtonUp(wxMouseEvent& event)
{
	m_ControlState = wxCONTROL_NONE;
	Refresh();

	wxPoint pos = event.GetPosition();
	if (m_IsSliptterEnabled && pos.x > (GetSize().GetWidth() - ms_ArrowButtonWidth))
	{
		CallAfter([this, pos]()
		{
			wxContextMenuEvent menuEvent(KxEVT_BUTTON_MENU, this->GetId());
			menuEvent.SetPosition(pos);
			menuEvent.SetEventObject(this);
			if ((!HandleWindowEvent(menuEvent) || menuEvent.GetSkipped()) && HasDropdownMenu())
			{
				GetDropdownMenu()->ShowAsPopup(this, 1);
			}
		});
	}
	#if 0
	else
	{
		wxCommandEvent clickEvent(KxEVT_BUTTON, this->GetId());
		clickEvent.SetEventObject(this);
		HandleWindowEvent(clickEvent);
	}
	#endif
	event.Skip();
}
void KxButton::OnLeftButtonDown(wxMouseEvent& event)
{
	m_ControlState = wxCONTROL_PRESSED;
	Refresh();
	event.Skip();
}

bool KxButton::Create(wxWindow* parent,
					   wxWindowID id,
					   const wxString& label,
					   const wxPoint& pos,
					   const wxSize& size,
					   long style,
					   const wxValidator& validator
)
{
	SetBackgroundStyle(wxBG_STYLE_PAINT);
	if (wxButton::Create(parent, id, label, pos, size, style, validator))
	{
		EnableSystemTheme();
		MakeOwnerDrawn();

		Bind(wxEVT_PAINT, &KxButton::OnPaint, this);
		Bind(wxEVT_LEFT_UP, &KxButton::OnLeftButtonUp, this);
		Bind(wxEVT_LEFT_DOWN, &KxButton::OnLeftButtonDown, this);
		Bind(wxEVT_LEAVE_WINDOW, &KxButton::OnMouseLeave, this);
		Bind(wxEVT_ENTER_WINDOW, &KxButton::OnMouseEnter, this);

		return true;
	}
	return false;
};
KxButton::~KxButton()
{
}

bool KxButton::Enable(bool isEnabled)
{
	m_ControlState = isEnabled ? wxCONTROL_NONE : wxCONTROL_DISABLED;
	isEnabled = wxButton::Enable(isEnabled);
	Refresh();
	return isEnabled;
}

wxSize KxButton::DoGetBestSize() const
{
	wxSize size = wxButton::DoGetBestSize();
	if (std::abs(size.y - ms_DefaultButtonHeight) <= GetCharHeight())
	{
		size.y = ms_DefaultButtonHeight;
	}
	return size;
}
wxSize KxButton::DoGetBestClientSize() const
{
	return wxButton::DoGetBestClientSize();
}

void KxButton::SetAuthNeeded(bool show)
{
	m_IsAuthNeeded = show;
	if (m_IsAuthNeeded)
	{
		KxLibrary library("ImageRes.dll", LOAD_LIBRARY_AS_DATAFILE);
		if (library.IsOK())
		{
			int iconSize = 16;
			int height = GetSize().GetHeight();
			if (height > 32)
			{
				iconSize = 32;
			}
			if (height > 48)
			{
				iconSize = 48;
			}
			if (height > 64)
			{
				iconSize = 64;
			}
			if (height > 128)
			{
				iconSize = 128;
			}
			if (height > 256)
			{
				iconSize = 256;
			}

			auto langs = library.EnumResourceLanguages(KxLibrary::ResIDToName(RT_GROUP_ICON), "78");
			if (!langs.empty())
			{
				wxBitmap bitmap;
				bitmap.CopyFromIcon(library.GetIcon("78", wxSize(iconSize, iconSize), langs[0]));
				SetBitmap(bitmap);
				return;
			}
		}
	}

	SetBitmap(wxNullBitmap);
	Refresh();
}

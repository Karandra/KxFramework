#include "KxStdAfx.h"
#include "KxFramework/KxButton.h"
#include "KxFramework/KxLibrary.h"
#include "KxFramework/KxMenu.h"
#include "KxFramework/KxUxTheme.h"
#include "KxFramework/KxIncludeWindows.h"

KxEVENT_DEFINE_GLOBAL_AS(wxCommandEvent, BUTTON, wxEVT_BUTTON);
KxEVENT_DEFINE_GLOBAL(wxContextMenuEvent, BUTTON_MENU);

wxIMPLEMENT_DYNAMIC_CLASS(KxButton, wxButton);

namespace
{
	constexpr auto g_DefaultButtonWidth = 72;
	constexpr auto g_DefaultButtonHeight = 23;
	constexpr auto g_ArrowButtonWidth = 17;

	int FromDIPX(const wxWindow* window, int value)
	{
		return window->FromDIP(wxSize(value, wxDefaultCoord)).GetWidth();
	}
	int FromDIPY(const wxWindow* window, int value)
	{
		return window->FromDIP(wxSize(wxDefaultCoord, value)).GetHeight();
	}

	wxSize CalcBestSize(const KxButton& button, wxSize size)
	{
		if (!size.IsFullySpecified())
		{
			size = button.GetSizeFromTextSize(button.GetTextExtent(button.GetLabelText()));
		}

		if (size.x <= 0)
		{
			size.x = FromDIPX(&button, g_DefaultButtonWidth);
		}
		if (size.y <= 0 || std::abs(size.y - g_DefaultButtonHeight) <= button.GetCharHeight())
		{
			size.y = FromDIPY(&button, g_DefaultButtonHeight);
		}
		return size;
	}
}

wxSize KxButton::GetDefaultSize()
{
	return wxButton::GetDefaultSize();
}

void KxButton::OnPaint(wxPaintEvent& event)
{
	wxAutoBufferedPaintDC dc(this);
	KxUxTheme::ClearDC(*this, dc);
	wxRendererNative& renderer = wxRendererNative::Get();

	const bool isEnabled = IsThisEnabled();
	const wxSize clientSize = GetSize();
	const wxRect contentRect = wxRect(FromDIP(wxPoint(2, 2)), clientSize - FromDIP(wxSize(4, 4)));
	int width = clientSize.GetWidth();
	int widthMod = 2;
	if (m_IsSliptterEnabled)
	{
		width -= g_ArrowButtonWidth;
		widthMod = 5;
	}
	wxRect rect(-1, -1, width + widthMod, clientSize.GetHeight() + 2);

	int controlState = m_ControlState;
	if (!isEnabled)
	{
		controlState |= wxCONTROL_DISABLED;
	}

	// Draw first part
	dc.SetTextForeground(isEnabled ? GetForegroundColour() : GetForegroundColour().MakeDisabled());
	renderer.DrawPushButton(this, dc, rect, controlState);

	// Draw focus rectangle
	if (m_IsFocusDrawingAllowed && HasFocus())
	{
		renderer.DrawFocusRect(this, dc, contentRect, wxCONTROL_SELECTED);
	}

	// Draw bitmap and label
	
	if (wxBitmap bitmap = GetBitmap(); bitmap.IsOk())
	{
		if (!isEnabled)
		{
			bitmap = bitmap.ConvertToDisabled();
		}
		dc.DrawLabel(GetLabelText(), bitmap, contentRect, wxALIGN_CENTER);
	}
	else
	{
		dc.DrawLabel(GetLabelText(), contentRect, wxALIGN_CENTER);
	}

	// Draw second part of the button
	if (m_IsSliptterEnabled)
	{
		wxRect splitRect = rect;
		splitRect.x = width + FromDIPX(this, 1);
		splitRect.y = -1;
		splitRect.width = FromDIPX(this, g_ArrowButtonWidth);
		splitRect.height = clientSize.GetHeight() + FromDIPX(this, 2);

		renderer.DrawPushButton(this, dc, splitRect, controlState);
		renderer.DrawDropArrow(this, dc, splitRect, controlState);
	}
}
void KxButton::OnResize(wxSizeEvent& event)
{
	ScheduleRefresh();
	event.Skip();
}
void KxButton::OnKillFocus(wxFocusEvent& event)
{
	ScheduleRefresh();
	m_ControlState = wxCONTROL_NONE;

	event.Skip();
}
void KxButton::OnMouseEnter(wxMouseEvent& event)
{
	ScheduleRefresh();
	m_ControlState = wxCONTROL_CURRENT;

	event.Skip();
}
void KxButton::OnMouseLeave(wxMouseEvent& event)
{
	ScheduleRefresh();
	m_ControlState = wxCONTROL_NONE;

	event.Skip();
}
void KxButton::OnLeftButtonUp(wxMouseEvent& event)
{
	ScheduleRefresh();
	m_ControlState = wxCONTROL_NONE;

	const wxPoint pos = event.GetPosition();
	if (m_IsSliptterEnabled && pos.x > (GetClientSize().GetWidth() - g_ArrowButtonWidth))
	{
		wxContextMenuEvent menuEvent(KxEVT_BUTTON_MENU, this->GetId());
		menuEvent.SetPosition(pos);
		menuEvent.SetEventObject(this);
		if ((!ProcessWindowEvent(menuEvent) || menuEvent.GetSkipped()) && HasDropdownMenu())
		{
			GetDropdownMenu()->ShowAsPopup(this, 1);
		}
	}
	else
	{
		wxCommandEvent clickEvent(KxEVT_BUTTON, this->GetId());
		clickEvent.SetEventObject(this);
		ProcessWindowEvent(clickEvent);
	}

	event.Skip();
}
void KxButton::OnLeftButtonDown(wxMouseEvent& event)
{
	ScheduleRefresh();
	m_ControlState = wxCONTROL_PRESSED;

	event.Skip();
}

wxSize KxButton::DoGetBestSize() const
{
	return CalcBestSize(*this, wxAnyButton::DoGetBestSize());
}
wxSize KxButton::DoGetBestClientSize() const
{
	return CalcBestSize(*this, wxAnyButton::DoGetBestClientSize());
}
wxSize KxButton::DoGetSizeFromTextSize(int xlen, int ylen) const
{
	wxSize size = ConvertDialogToPixels(wxSize(16, 0));
	if (xlen > 0)
	{
		size.x += xlen;
	}
	if (ylen > 0)
	{
		size.y += ylen;
	}
	return size;
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
	if (wxAnyButton::Create(parent, id, pos, size, style, validator))
	{
		SetLabel(label);
		SetBackgroundStyle(wxBG_STYLE_PAINT);
		EnableSystemTheme();
		MakeOwnerDrawn();

		m_EventHandler.SetClientData(this);
		m_EventHandler.Bind(wxEVT_PAINT, &KxButton::OnPaint, this);
		m_EventHandler.Bind(wxEVT_SIZE, &KxButton::OnResize, this);
		m_EventHandler.Bind(wxEVT_KILL_FOCUS, &KxButton::OnKillFocus, this);
		m_EventHandler.Bind(wxEVT_LEFT_UP, &KxButton::OnLeftButtonUp, this);
		m_EventHandler.Bind(wxEVT_LEFT_DOWN, &KxButton::OnLeftButtonDown, this);
		m_EventHandler.Bind(wxEVT_LEAVE_WINDOW, &KxButton::OnMouseLeave, this);
		m_EventHandler.Bind(wxEVT_ENTER_WINDOW, &KxButton::OnMouseEnter, this);
		PushEventHandler(&m_EventHandler);

		return true;
	}
	return false;
};
KxButton::~KxButton()
{
	if (m_EventHandler.GetClientData() == this)
	{
		PopEventHandler();
	}
}

bool KxButton::Enable(bool isEnabled)
{
	ScheduleRefresh();
	return wxAnyButton::Enable(isEnabled);
}
void KxButton::SetLabel(const wxString& label)
{
	ScheduleRefresh();
	wxAnyButton::SetLabel(label);
}

bool KxButton::IsDefault() const
{
	wxWindow* tlwParent = wxGetTopLevelParent(const_cast<KxButton*>(this));
	if (tlwParent && tlwParent->IsKindOf(wxCLASSINFO(wxTopLevelWindow)))
	{
		return static_cast<wxTopLevelWindow*>(tlwParent)->GetDefaultItem() == this;
	}
	return false;
}
wxWindow* KxButton::SetDefault()
{
	ScheduleRefresh();

	wxWindow* tlwParent = wxGetTopLevelParent(this);
	if (tlwParent && tlwParent->IsKindOf(wxCLASSINFO(wxTopLevelWindow)))
	{
		return static_cast<wxTopLevelWindow*>(tlwParent)->SetDefaultItem(this);
	}
	return nullptr;
}

void KxButton::SetAuthNeeded(bool show)
{
	ScheduleRefresh();
	m_IsAuthNeeded = show;

	if (m_IsAuthNeeded)
	{
		KxLibrary library("ImageRes.dll", LOAD_LIBRARY_AS_DATAFILE);
		if (library.IsOK())
		{
			int iconSize = 16;
			const int height = GetSize().GetHeight();
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
			if (height > 512)
			{
				iconSize = 512;
			}

			auto langs = library.EnumResourceLanguages(KxLibrary::ResIDToName(RT_GROUP_ICON), wxS("78"));
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
}

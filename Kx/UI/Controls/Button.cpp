#include "stdafx.h"
#include "Button.h"
#include "Kx/Drawing/UxTheme.h"
#include "Kx/UI/Menus/Menu.h"
#include "Kx/System/DynamicLibrary.h"
#include "Kx/System/Private/System.h"
#include <wx/toplevel.h>
#include <wx/button.h>

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

	KxFramework::Size CalcBestSize(const KxFramework::UI::Button& button, KxFramework::Size size)
	{
		if (!size.IsFullySpecified())
		{
			size = button.GetSizeFromTextSize(button.GetTextExtent(button.GetLabelText()));
		}

		if (size.GetWidth() <= 0)
		{
			size.SetWidth(FromDIPX(&button, g_DefaultButtonWidth));
		}
		if (size.GetHeight() <= 0 || std::abs(size.GetHeight() - g_DefaultButtonHeight) <= button.GetCharHeight())
		{
			size.SetHeight(FromDIPY(&button, g_DefaultButtonHeight));
		}
		return size;
	}
}

namespace KxFramework::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(Button, wxAnyButton);

	Size Button::GetDefaultSize()
	{
		return wxButton::GetDefaultSize();
	}

	void Button::OnPaint(wxPaintEvent& event)
	{
		using namespace KxFramework;

		wxAutoBufferedPaintDC dc(this);
		UxTheme::ClearDC(*this, dc);
		wxRendererNative& renderer = wxRendererNative::Get();

		const bool isEnabled = IsThisEnabled();
		const Size clientSize = GetSize();
		const Rect contentRect = Rect(FromDIP(Point(2, 2)), (wxSize)clientSize - FromDIP(wxSize(4, 4)));
		int width = clientSize.GetWidth();
		int widthMod = 2;
		if (m_IsSliptterEnabled)
		{
			width -= g_ArrowButtonWidth;
			widthMod = 5;
		}
		Rect rect(-1, -1, width + widthMod, clientSize.GetHeight() + 2);

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
			dc.DrawLabel(GetLabelText(), bitmap, contentRect, ToInt(Alignment::Center));
		}
		else
		{
			dc.DrawLabel(GetLabelText(), contentRect, ToInt(Alignment::Center));
		}

		// Draw second part of the button
		if (m_IsSliptterEnabled)
		{
			Rect splitRect = rect;
			splitRect.X() = width + FromDIPX(this, 1);
			splitRect.Y() = -1;
			splitRect.Width() = FromDIPX(this, g_ArrowButtonWidth);
			splitRect.Height() = clientSize.GetHeight() + FromDIPX(this, 2);

			renderer.DrawPushButton(this, dc, splitRect, controlState);
			renderer.DrawDropArrow(this, dc, splitRect, controlState);
		}
	}
	void Button::OnResize(wxSizeEvent& event)
	{
		ScheduleRefresh();
		event.Skip();
	}
	void Button::OnKillFocus(wxFocusEvent& event)
	{
		ScheduleRefresh();
		m_ControlState = wxCONTROL_NONE;

		event.Skip();
	}
	void Button::OnMouseEnter(wxMouseEvent& event)
	{
		ScheduleRefresh();
		m_ControlState = wxCONTROL_CURRENT;

		event.Skip();
	}
	void Button::OnMouseLeave(wxMouseEvent& event)
	{
		ScheduleRefresh();
		m_ControlState = wxCONTROL_NONE;

		event.Skip();
	}
	void Button::OnLeftButtonUp(wxMouseEvent& event)
	{
		ScheduleRefresh();
		m_ControlState = wxCONTROL_NONE;

		const Point pos = event.GetPosition();
		if (m_IsSliptterEnabled && pos.GetX() > (GetClientSize().GetWidth() - g_ArrowButtonWidth))
		{
			wxContextMenuEvent menuEvent(EvtMenu, this->GetId());
			menuEvent.SetPosition(pos);
			menuEvent.SetEventObject(this);
			if ((!ProcessWindowEvent(menuEvent) || menuEvent.GetSkipped()) && HasDropdownMenu())
			{
				GetDropdownMenu()->ShowAsPopup(this, 1);
			}
		}
		else
		{
			wxCommandEvent clickEvent(EvtClick, this->GetId());
			clickEvent.SetEventObject(this);
			ProcessWindowEvent(clickEvent);
		}

		event.Skip();
	}
	void Button::OnLeftButtonDown(wxMouseEvent& event)
	{
		ScheduleRefresh();
		m_ControlState = wxCONTROL_PRESSED;

		event.Skip();
	}

	wxSize Button::DoGetBestSize() const
	{
		return CalcBestSize(*this, wxAnyButton::DoGetBestSize());
	}
	wxSize Button::DoGetBestClientSize() const
	{
		return CalcBestSize(*this, wxAnyButton::DoGetBestClientSize());
	}
	wxSize Button::DoGetSizeFromTextSize(int xlen, int ylen) const
	{
		Size size = ConvertDialogToPixels(wxSize(16, 0));
		if (xlen > 0)
		{
			size.Width() += xlen;
		}
		if (ylen > 0)
		{
			size.Height() += ylen;
		}
		return size;
	}

	bool Button::Create(wxWindow* parent,
						wxWindowID id,
						const String& label,
						const Point& pos,
						const Size& size,
						FlagSet<ButtonStyle> style,
						const wxValidator& validator
	)
	{
		
		if (wxAnyButton::Create(parent, id, pos, size, style.ToInt(), validator))
		{
			SetLabel(label);
			SetBackgroundStyle(wxBG_STYLE_PAINT);
			EnableSystemTheme();
			MakeOwnerDrawn();

			PushEventHandler(&m_EventHandler);
			m_EventHandler.SetClientData(this);

			m_EventHandler.Bind(wxEVT_PAINT, &Button::OnPaint, this);
			m_EventHandler.Bind(wxEVT_SIZE, &Button::OnResize, this);
			m_EventHandler.Bind(wxEVT_KILL_FOCUS, &Button::OnKillFocus, this);
			m_EventHandler.Bind(wxEVT_LEFT_UP, &Button::OnLeftButtonUp, this);
			m_EventHandler.Bind(wxEVT_LEFT_DOWN, &Button::OnLeftButtonDown, this);
			m_EventHandler.Bind(wxEVT_LEAVE_WINDOW, &Button::OnMouseLeave, this);
			m_EventHandler.Bind(wxEVT_ENTER_WINDOW, &Button::OnMouseEnter, this);

			return true;
		}
		return false;
	};

	bool Button::IsDefault() const
	{
		const wxWindow* tlwParent = wxGetTopLevelParent(const_cast<Button*>(this));
		if (tlwParent && tlwParent->IsKindOf(wxCLASSINFO(wxTopLevelWindow)))
		{
			return static_cast<const wxTopLevelWindow*>(tlwParent)->GetDefaultItem() == this;
		}
		return false;
	}
	wxWindow* Button::SetDefault()
	{
		ScheduleRefresh();

		wxWindow* tlwParent = wxGetTopLevelParent(this);
		if (tlwParent && tlwParent->IsKindOf(wxCLASSINFO(wxTopLevelWindow)))
		{
			return static_cast<wxTopLevelWindow*>(tlwParent)->SetDefaultItem(this);
		}
		return nullptr;
	}

	void Button::SetAuthNeeded(bool show)
	{
		ScheduleRefresh();
		m_IsAuthNeeded = show;

		if (m_IsAuthNeeded)
		{
			DynamicLibrary library(wxS("ImageRes.dll"), DynamicLibraryLoadFlag::DataFile);
			if (library)
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

				wxIcon icon = library.GetIconResource(wxS("78"), Size(iconSize, iconSize));
				if (icon.IsOk())
				{
					SetBitmap(Drawing::ToBitmap(icon));
					return;
				}
			}
		}

		SetBitmap(wxNullBitmap);
	}
}

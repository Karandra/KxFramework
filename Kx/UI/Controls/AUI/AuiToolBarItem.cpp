#include "stdafx.h"
#include "AuiToolBarItem.h"
#include "AuiToolBar.h"
#include "Kx/Utility/Common.h"
#include "Kx/UI/Menus/Menu.h"

namespace KxFramework::UI
{
	Point AuiToolBarItem::DoGetDropdownMenuPosition(wxAlignment* alignment, bool leftAlign) const
	{
		Rect rect = GetRect();
		if (!rect.IsEmpty())
		{
			if (leftAlign)
			{
				Utility::SetIfNotNull(alignment, static_cast<wxAlignment>(wxALIGN_LEFT|wxALIGN_TOP));
				return rect.GetLeftBottom() + Point(0, 2);
			}
			else
			{
				Utility::SetIfNotNull(alignment, static_cast<wxAlignment>(wxALIGN_RIGHT|wxALIGN_TOP));
				return rect.GetRightBottom() + Point(0, 2);
			}
		}
		return wxDefaultPosition;
	}
	wxWindowID AuiToolBarItem::DoShowDropdownMenu(bool leftAlign)
	{
		wxAlignment alignment = Menu::DefaultAlignment;
		Point pos = DoGetDropdownMenuPosition(&alignment, leftAlign);
		wxWindowID ret = GetDropdownMenu()->Show(m_ToolBar, pos, alignment);

		// To make parent window respond to mouse events without clicking to window manually
		m_ToolBar->GenerateMouseLeave();
		m_ToolBar->GetParent()->CaptureMouse();
		m_ToolBar->GetParent()->ReleaseMouse();
		return ret;
	}

	void AuiToolBarItem::Refresh()
	{
		m_ToolBar->RefreshRect(GetRect());
	}

	wxWindowID AuiToolBarItem::GetID() const
	{
		return m_Item->GetId();
	}
	Rect AuiToolBarItem::GetRect() const
	{
		return m_ToolBar->wxAuiToolBar::GetToolRect(GetID());
	}
	bool AuiToolBarItem::IsItemFits() const
	{
		return m_ToolBar->wxAuiToolBar::GetToolFits(GetID());
	}

	int AuiToolBarItem::GetIndex() const
	{
		return m_ToolBar->DoGetToolIndex(*this);
	}
	bool AuiToolBarItem::SetIndex(size_t index)
	{
		return m_ToolBar->DoSetToolIndex(*this, index);
	}

	bool AuiToolBarItem::IsToggled() const
	{
		return m_ToolBar->wxAuiToolBar::GetToolToggled(GetID());
	}
	void AuiToolBarItem::SetToggled(bool isPressed)
	{
		m_ToolBar->wxAuiToolBar::ToggleTool(GetID(), isPressed);
		Refresh();
	}

	bool AuiToolBarItem::IsEnabled() const
	{
		return m_ToolBar->wxAuiToolBar::GetToolEnabled(GetID());
	}
	void AuiToolBarItem::SetEnabled(bool isEnabled)
	{
		m_ToolBar->wxAuiToolBar::EnableTool(GetID(), isEnabled);
		Refresh();
	}

	bool AuiToolBarItem::HasDropDown() const
	{
		return m_Item->HasDropDown();
	}
	void AuiToolBarItem::SetDropDown(bool isDropDown)
	{
		m_Item->SetHasDropDown(isDropDown);
	}

	bool AuiToolBarItem::IsSticky() const
	{
		return m_Item->IsSticky();
	}
	void AuiToolBarItem::SetSticky(bool isSticky)
	{
		m_Item->SetSticky(isSticky);
	}

	void AuiToolBarItem::SetActive(bool isActive)
	{
		m_Item->SetActive(isActive);
	}
	bool AuiToolBarItem::IsActive() const
	{
		return m_Item->IsActive();
	}

	int AuiToolBarItem::GetProportion() const
	{
		return m_ToolBar->wxAuiToolBar::GetToolProportion(GetID());
	}
	void AuiToolBarItem::SetProportion(int proportion)
	{
		m_ToolBar->wxAuiToolBar::SetToolProportion(GetID(), proportion);
	}

	wxAlignment AuiToolBarItem::GetAlignment() const
	{
		return static_cast<wxAlignment>(m_Item->GetAlignment());
	}
	void AuiToolBarItem::SetAlignment(wxAlignment alignment)
	{
		m_Item->SetAlignment(alignment);
	}

	int AuiToolBarItem::GetSpacerPixels() const
	{
		return m_Item->GetSpacerPixels();
	}
	void AuiToolBarItem::SetSpacerPixels(int pixels)
	{
		m_Item->SetSpacerPixels(pixels);
	}

	void AuiToolBarItem::SetKind(wxItemKind kind)
	{
		m_Item->SetKind(kind);
	}
	wxItemKind AuiToolBarItem::GetKind() const
	{
		return static_cast<wxItemKind>(m_Item->GetKind());
	}

	void AuiToolBarItem::SetWindow(wxWindow* window)
	{
		return m_Item->SetWindow(window);
	}
	wxWindow* AuiToolBarItem::GetWindow()
	{
		return m_Item->GetWindow();
	}

	const String& AuiToolBarItem::GetLabel() const
	{
		return m_Item->GetLabel();
	}
	void AuiToolBarItem::SetLabel(const String& label)
	{
		m_Item->SetLabel(label);
	}

	const String& AuiToolBarItem::GetShortHelp() const
	{
		return m_Item->GetShortHelp();
	}
	void AuiToolBarItem::SetShortHelp(const String& helpString)
	{
		m_Item->SetShortHelp(helpString);
	}

	const String& AuiToolBarItem::GetLongHelp() const
	{
		return m_Item->GetLongHelp();
	}
	void AuiToolBarItem::SetLongHelp(const String& helpString)
	{
		m_Item->SetLongHelp(helpString);
	}

	const wxBitmap& AuiToolBarItem::GetBitmap() const
	{
		return m_Item->GetBitmap();
	}
	void AuiToolBarItem::SetBitmap(const wxBitmap& bitmap)
	{
		m_Item->SetBitmap(bitmap);
		Refresh();
	}

	const wxBitmap& AuiToolBarItem::GetDisabledBitmap() const
	{
		return m_Item->GetDisabledBitmap();
	}
	void AuiToolBarItem::SetDisabledBitmap(const wxBitmap& bitmap)
	{
		m_Item->SetDisabledBitmap(bitmap);
		Refresh();
	}
}

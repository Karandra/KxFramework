#include "KxStdAfx.h"
#include "KxFramework/KxAuiToolBarItem.h"
#include "KxFramework/KxAuiToolBar.h"
#include "KxFramework/KxMenu.h"
#include "Kx/Utility/Common.h"

using namespace KxFramework;

wxPoint KxAuiToolBarItem::DoGetDropdownMenuPosition(DWORD* alignment, bool leftAlign) const
{
	wxRect rect = GetRect();
	if (!rect.IsEmpty())
	{
		if (leftAlign)
		{
			Utility::SetIfNotNull(alignment, TPM_LEFTALIGN|TPM_TOPALIGN);
			return rect.GetLeftBottom() + wxPoint(0, 2);
		}
		else
		{
			Utility::SetIfNotNull(alignment, TPM_RIGHTALIGN|TPM_TOPALIGN);
			return rect.GetRightBottom() + wxPoint(0, 2);
		}
	}
	return wxDefaultPosition;
}
wxWindowID KxAuiToolBarItem::DoShowDropdownMenu(bool leftAlign)
{
	DWORD alignment = 0;
	wxPoint pos = DoGetDropdownMenuPosition(&alignment, leftAlign);
	wxWindowID ret = GetDropdownMenu()->Show(m_ToolBar, pos, alignment);

	// To make parent window respond to mouse events without clicking to window manually
	m_ToolBar->GenerateMouseLeave();
	m_ToolBar->GetParent()->CaptureMouse();
	m_ToolBar->GetParent()->ReleaseMouse();
	return ret;
}

KxAuiToolBarItem::KxAuiToolBarItem(KxAuiToolBar& control, wxAuiToolBarItem& item)
	:m_ToolBar(&control), m_Item(&item)
{
}
KxAuiToolBarItem::~KxAuiToolBarItem()
{
}

bool KxAuiToolBarItem::IsOK() const
{
	return m_ToolBar != nullptr && m_Item != nullptr;
}
void KxAuiToolBarItem::Refresh()
{
	m_ToolBar->RefreshRect(GetRect());
}

wxWindowID KxAuiToolBarItem::GetID() const
{
	return m_Item->GetId();
}
wxRect KxAuiToolBarItem::GetRect() const
{
	return m_ToolBar->wxAuiToolBar::GetToolRect(GetID());
}
bool KxAuiToolBarItem::IsItemFits() const
{
	return m_ToolBar->wxAuiToolBar::GetToolFits(GetID());
}

int KxAuiToolBarItem::GetIndex() const
{
	return m_ToolBar->DoGetToolIndex(*this);
}
bool KxAuiToolBarItem::SetIndex(size_t index)
{
	return m_ToolBar->DoSetToolIndex(*this, index);
}

bool KxAuiToolBarItem::IsToggled() const
{
	return m_ToolBar->wxAuiToolBar::GetToolToggled(GetID());
}
void KxAuiToolBarItem::SetToggled(bool isPressed)
{
	m_ToolBar->wxAuiToolBar::ToggleTool(GetID(), isPressed);
	Refresh();
}

bool KxAuiToolBarItem::IsEnabled() const
{
	return m_ToolBar->wxAuiToolBar::GetToolEnabled(GetID());
}
void KxAuiToolBarItem::SetEnabled(bool isEnabled)
{
	m_ToolBar->wxAuiToolBar::EnableTool(GetID(), isEnabled);
	Refresh();
}

bool KxAuiToolBarItem::HasDropDown() const
{
	return m_Item->HasDropDown();
}
void KxAuiToolBarItem::SetDropDown(bool isDropDown)
{
	m_Item->SetHasDropDown(isDropDown);
}

bool KxAuiToolBarItem::IsSticky() const
{
	return m_Item->IsSticky();
}
void KxAuiToolBarItem::SetSticky(bool isSticky)
{
	m_Item->SetSticky(isSticky);
}

void KxAuiToolBarItem::SetActive(bool isActive)
{
	m_Item->SetActive(isActive);
}
bool KxAuiToolBarItem::IsActive() const
{
	return m_Item->IsActive();
}

int KxAuiToolBarItem::GetProportion() const
{
	return m_ToolBar->wxAuiToolBar::GetToolProportion(GetID());
}
void KxAuiToolBarItem::SetProportion(int proportion)
{
	m_ToolBar->wxAuiToolBar::SetToolProportion(GetID(), proportion);
}

wxAlignment KxAuiToolBarItem::GetAlignment() const
{
	return static_cast<wxAlignment>(m_Item->GetAlignment());
}
void KxAuiToolBarItem::SetAlignment(wxAlignment alignment)
{
	m_Item->SetAlignment(alignment);
}

int KxAuiToolBarItem::GetSpacerPixels() const
{
	return m_Item->GetSpacerPixels();
}
void KxAuiToolBarItem::SetSpacerPixels(int pixels)
{
	m_Item->SetSpacerPixels(pixels);
}

void KxAuiToolBarItem::SetKind(wxItemKind kind)
{
	m_Item->SetKind(kind);
}
wxItemKind KxAuiToolBarItem::GetKind() const
{
	return static_cast<wxItemKind>(m_Item->GetKind());
}

void KxAuiToolBarItem::SetWindow(wxWindow* window)
{
	return m_Item->SetWindow(window);
}
wxWindow* KxAuiToolBarItem::GetWindow()
{
	return m_Item->GetWindow();
}

const wxString& KxAuiToolBarItem::GetLabel() const
{
	return m_Item->GetLabel();
}
void KxAuiToolBarItem::SetLabel(const wxString& label)
{
	m_Item->SetLabel(label);
}

const wxString& KxAuiToolBarItem::GetShortHelp() const
{
	return m_Item->GetShortHelp();
}
void KxAuiToolBarItem::SetShortHelp(const wxString& helpString)
{
	m_Item->SetShortHelp(helpString);
}

const wxString& KxAuiToolBarItem::GetLongHelp() const
{
	return m_Item->GetLongHelp();
}
void KxAuiToolBarItem::SetLongHelp(const wxString& helpString)
{
	m_Item->SetLongHelp(helpString);
}

const wxBitmap& KxAuiToolBarItem::GetBitmap() const
{
	return m_Item->GetBitmap();
}
void KxAuiToolBarItem::SetBitmap(const wxBitmap& bitmap)
{
	m_Item->SetBitmap(bitmap);
	Refresh();
}

const wxBitmap& KxAuiToolBarItem::GetDisabledBitmap() const
{
	return m_Item->GetDisabledBitmap();
}
void KxAuiToolBarItem::SetDisabledBitmap(const wxBitmap& bitmap)
{
	m_Item->SetDisabledBitmap(bitmap);
	Refresh();
}

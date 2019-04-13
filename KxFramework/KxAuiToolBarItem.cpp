#include "KxStdAfx.h"
#include "KxFramework/KxAuiToolBarItem.h"
#include "KxFramework/KxAuiToolBar.h"
#include "KxFramework/KxMenu.h"
#include "KxFramework/KxUtility.h"

wxPoint KxAuiToolBarItem::DoGetDropdownMenuPosition(DWORD* alignment, bool leftAlign) const
{
	wxRect rect = GetRect();
	if (!rect.IsEmpty())
	{
		if (leftAlign)
		{
			KxUtility::SetIfNotNull(alignment, TPM_LEFTALIGN|TPM_TOPALIGN);
			return rect.GetLeftBottom() + wxPoint(0, 2);
		}
		else
		{
			KxUtility::SetIfNotNull(alignment, TPM_RIGHTALIGN|TPM_TOPALIGN);
			return rect.GetRightBottom() + wxPoint(0, 2);
		}
	}
	return wxDefaultPosition;
}
wxWindowID KxAuiToolBarItem::DoShowDropdownMenu(bool leftAlign)
{
	DWORD alignment = 0;
	wxPoint pos = DoGetDropdownMenuPosition(&alignment, leftAlign);
	wxWindowID ret = GetDropdownMenu()->Show(m_Control, pos, alignment);

	// To make parent window respond to mouse events without clicking to window manually
	m_Control->GenerateMouseLeave();
	m_Control->GetParent()->CaptureMouse();
	m_Control->GetParent()->ReleaseMouse();
	return ret;
}

KxAuiToolBarItem::KxAuiToolBarItem(KxAuiToolBar* control, wxAuiToolBarItem* item)
	:m_Control(control), m_Item(item)
{
}
KxAuiToolBarItem::~KxAuiToolBarItem()
{
}

bool KxAuiToolBarItem::IsOK() const
{
	return m_Control != nullptr && m_Item != nullptr;
}
void KxAuiToolBarItem::Refresh()
{
	m_Control->RefreshRect(GetRect(), true);
}

KxAuiToolBar* KxAuiToolBarItem::GetToolBar() const
{
	return m_Control;
}
wxWindowID KxAuiToolBarItem::GetID() const
{
	return m_Item->GetId();
}
wxRect KxAuiToolBarItem::GetRect() const
{
	return m_Control->wxAuiToolBar::GetToolRect(GetID());
}
int KxAuiToolBarItem::GetPosition() const
{
	return m_Control->wxAuiToolBar::GetToolPos(GetID());
}
int KxAuiToolBarItem::GetIndex() const
{
	return m_Control->wxAuiToolBar::GetToolIndex(GetID());
}
bool KxAuiToolBarItem::IsItemFits() const
{
	return m_Control->wxAuiToolBar::GetToolFits(GetID());
}

bool KxAuiToolBarItem::IsToggled() const
{
	return m_Control->wxAuiToolBar::GetToolToggled(GetID());
}
void KxAuiToolBarItem::SetToggled(bool isPressed)
{
	m_Control->wxAuiToolBar::ToggleTool(GetID(), isPressed);
	Refresh();
}
bool KxAuiToolBarItem::IsEnabled() const
{
	return m_Control->wxAuiToolBar::GetToolEnabled(GetID());
}
void KxAuiToolBarItem::SetEnabled(bool isEnabled)
{
	m_Control->wxAuiToolBar::EnableTool(GetID(), isEnabled);
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
	return m_Control->wxAuiToolBar::GetToolProportion(GetID());
}
void KxAuiToolBarItem::SetProportion(int proportion)
{
	m_Control->wxAuiToolBar::SetToolProportion(GetID(), proportion);
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

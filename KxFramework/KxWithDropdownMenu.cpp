#include "KxStdAfx.h"
#include "KxFramework/KxWithDropdownMenu.h"
#include "KxFramework/KxMenu.h"

void KxWithDropdownMenu::DeleteIfNeeded()
{
	if (m_IsOwned)
	{
		delete m_Menu;
	}
}

KxWithDropdownMenu::KxWithDropdownMenu()
{
}
KxWithDropdownMenu::~KxWithDropdownMenu()
{
	DeleteIfNeeded();
}

void KxWithDropdownMenu::SetDropdownMenu(const KxMenu* menu)
{
	DeleteIfNeeded();
	m_IsOwned = false;
	m_Menu = const_cast<KxMenu*>(menu);
}
void KxWithDropdownMenu::AssignDropdownMenu(KxMenu* menu)
{
	DeleteIfNeeded();
	m_IsOwned = true;
	m_Menu = menu;
}

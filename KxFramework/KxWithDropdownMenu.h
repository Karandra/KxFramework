#pragma once
#include "KxFramework/KxFramework.h"
class KxMenu;

class KxWithDropdownMenu
{
	private:
		KxMenu* m_Menu = NULL;
		bool m_IsOwned = false;

	private:
		void DeleteIfNeeded();

	public:
		KxWithDropdownMenu();
		virtual ~KxWithDropdownMenu();

	public:
		bool HasDropdownMenu() const
		{
			return m_Menu != NULL;
		}
		bool OwnsDropdownMenu() const
		{
			return m_IsOwned;
		}

		KxMenu* GetDropdownMenu()
		{
			return m_Menu;
		}
		const KxMenu* GetDropdownMenu() const
		{
			return m_Menu;
		}
		void SetDropdownMenu(const KxMenu* menu);
		void AssignDropdownMenu(KxMenu* menu);
};

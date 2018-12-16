/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"
class KX_API KxMenu;

class KX_API KxWithDropdownMenu
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

#pragma once
#include "Common.h"
#include "KxFramework/KxMenu.h"

namespace KxFramework
{
	class KX_API WithDropdownMenu
	{
		private:
			KxMenu* m_Menu = nullptr;
			bool m_IsOwned = false;

		private:
			void DeleteIfNeeded()
			{
				if (m_IsOwned)
				{
					delete m_Menu;
				}

				m_Menu = nullptr;
				m_IsOwned = false;
			}

		public:
			WithDropdownMenu() = default;
			virtual ~WithDropdownMenu()
			{
				DeleteIfNeeded();
			}

		public:
			bool HasDropdownMenu() const
			{
				return m_Menu != nullptr;
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
			virtual void SetDropdownMenu(const KxMenu* menu)
			{
				DeleteIfNeeded();
				m_IsOwned = false;
				m_Menu = const_cast<KxMenu*>(menu);
			}
			virtual void AssignDropdownMenu(KxMenu* menu)
			{
				DeleteIfNeeded();
				m_IsOwned = true;
				m_Menu = menu;
			}
	};
}

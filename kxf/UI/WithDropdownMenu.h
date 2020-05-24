#pragma once
#include "Common.h"
#include "kxf/UI/Menus/Menu.h"

namespace kxf::UI
{
	class KX_API WithDropdownMenu
	{
		private:
			Menu* m_Menu = nullptr;
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

			Menu* GetDropdownMenu()
			{
				return m_Menu;
			}
			const Menu* GetDropdownMenu() const
			{
				return m_Menu;
			}
			virtual void SetDropdownMenu(const Menu* menu)
			{
				DeleteIfNeeded();
				m_IsOwned = false;
				m_Menu = const_cast<Menu*>(menu);
			}
			virtual void AssignDropdownMenu(Menu* menu)
			{
				DeleteIfNeeded();
				m_IsOwned = true;
				m_Menu = menu;
			}
	};
}

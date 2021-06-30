#include "KxfPCH.h"
#include "MenuWidgetItem.h"
#include "MenuWidget.h"
#include "kxf/Drawing/BitmapImage.h"
#include "kxf/Drawing/GDIRenderer/GDIBitmap.h"
#include "WXUI/Menu.h"
#include "WXUI/MenuItem.h"
#include <wx/menu.h>
#include <wx/colour.h>
#include <Windows.h>

namespace kxf::Widgets
{
	// MenuWidgetItem
	void MenuWidgetItem::DoCreateWidget()
	{
		MenuWidget::AssociateWXMenuItem(*m_MenuItem, *this);
	}
	bool MenuWidgetItem::DoDestroyWidget(bool releaseWX)
	{
		if (m_MenuItem)
		{
			auto menuItem = std::move(m_MenuItem);
			MenuWidget::DissociateWXMenuItem(*menuItem);

			if (releaseWX)
			{
				static_cast<void>(menuItem.release());
			}
			return true;
		}
		return false;
	}
	void MenuWidgetItem::OnWXMenuDestroyed()
	{
		DoDestroyWidget(true);
	}

	MenuWidgetItem::MenuWidgetItem()
	{
	}
	MenuWidgetItem::~MenuWidgetItem()
	{
		DoDestroyWidget();
	}

	// --- IMenuWidgetItem ---
	// General
	std::shared_ptr<IMenuWidget> MenuWidgetItem::GetSubMenu() const
	{
		if (m_MenuItem)
		{
			if (auto subMenu = m_MenuItem->GetSubMenu())
			{
				object_ptr<IMenuWidget> menu;
				auto widget = Private::FindByWXObject(*subMenu);
				if (widget && widget->QueryInterface(menu))
				{
					return menu->LockMenuReference();
				}
			}
		}
		return nullptr;
	}
	void MenuWidgetItem::SetSubMenu(IMenuWidget& subMenu)
	{
		object_ptr<MenuWidget> menuWidget;
		if (m_MenuItem && subMenu.IsWidgetAlive() && subMenu.QueryInterface(menuWidget) && !menuWidget->m_IsAttached)
		{
			m_MenuItem->SetSubMenu(menuWidget->m_Menu.get());
		}
	}

	bool MenuWidgetItem::IsDefaultItem() const
	{
		if (m_MenuItem)
		{
			if (auto menu = m_OwningMenu.lock())
			{
				return menu->GetDefaultItem().get() == this;
			}
		}
		return false;
	}
	void MenuWidgetItem::SetDefaultItem()
	{
		if (m_MenuItem)
		{
			if (auto menu = m_OwningMenu.lock())
			{
				::SetMenuDefaultItem(reinterpret_cast<HMENU>(menu->GetHandle()), WXUI::Menu::WxIDToWin(m_MenuItem->GetId()), FALSE);
			}
		}
	}

	// Properties
	MenuWidgetItemType MenuWidgetItem::GetItemType() const
	{
		if (m_MenuItem)
		{
			if (m_MenuItem->IsSubMenu())
			{
				return MenuWidgetItemType::SubMenu;
			}

			switch (m_MenuItem->GetKind())
			{
				case wxITEM_NORMAL:
				{
					return MenuWidgetItemType::Regular;
				}
				case wxITEM_SEPARATOR:
				{
					return MenuWidgetItemType::Separator;
				}
				case wxITEM_CHECK:
				{
					return MenuWidgetItemType::CheckItem;
				}
				case wxITEM_RADIO:
				{
					return MenuWidgetItemType::RadioItem;
				}
			};
		}
		return MenuWidgetItemType::None;
	}

	String MenuWidgetItem::GetLabel(FlagSet<WidgetTextFlag> flags) const
	{
		if (m_MenuItem && !m_MenuItem->IsSeparator())
		{
			if (flags.Contains(WidgetTextFlag::WithMnemonics))
			{
				return m_MenuItem->GetItemLabel();
			}
			else
			{
				return m_MenuItem->GetItemLabelText();
			}
		}
		return {};
	}
	void MenuWidgetItem::SetLabel(const String& label, FlagSet<WidgetTextFlag> flags)
	{
		if (m_MenuItem && !m_MenuItem->IsSeparator())
		{
			m_MenuItem->SetItemLabel(label);
		}
	}

	String MenuWidgetItem::GetDescription() const
	{
		if (m_MenuItem && !m_MenuItem->IsSeparator())
		{
			return m_MenuItem->GetHelp();
		}
		return {};
	}
	void MenuWidgetItem::SetDescription(const String& description)
	{
		if (m_MenuItem && !m_MenuItem->IsSeparator())
		{
			m_MenuItem->SetHelp(description);
		}
	}

	WidgetID MenuWidgetItem::GetItemID() const
	{
		if (m_MenuItem)
		{
			if (m_MenuItem->IsSeparator())
			{
				return StdID::Separator;
			}
			else
			{
				return m_ItemID;
			}
		}
		return {};
	}
	void MenuWidgetItem::SetItemID(WidgetID id)
	{
		if (m_MenuItem && !m_MenuItem->IsSeparator())
		{
			m_ItemID = id;
		}
	}

	BitmapImage MenuWidgetItem::GetItemIcon() const
	{
		if (m_MenuItem)
		{
			return GDIBitmap(m_MenuItem->GetBitmap(false));
		}
		return {};
	}
	void MenuWidgetItem::SetItemIcon(const BitmapImage& icon)
	{
		if (m_MenuItem)
		{
			// Don't support checked bitmap for now 
			BitmapImage checked;

			if (m_MenuItem->IsCheckable())
			{
				if (checked)
				{
					m_MenuItem->SetBitmaps(icon.ToGDIBitmap().ToWxBitmap(), checked.ToGDIBitmap().ToWxBitmap());
				}
				else
				{
					m_MenuItem->SetBitmap(icon.ToGDIBitmap().ToWxBitmap(), false);
				}
			}
			else
			{
				m_MenuItem->SetBitmaps(icon.ToGDIBitmap().ToWxBitmap(), checked.ToGDIBitmap().ToWxBitmap());
			}
		}
	}

	bool MenuWidgetItem::IsEnabled() const
	{
		if (m_MenuItem)
		{
			return m_MenuItem->IsEnabled();
		}
		return false;
	}
	void MenuWidgetItem::SetEnabled(bool enabled)
	{
		if (m_MenuItem)
		{
			m_MenuItem->Enable(enabled);
		}
	}

	bool MenuWidgetItem::IsChecked() const
	{
		if (m_MenuItem)
		{
			return m_MenuItem->IsChecked();
		}
		return false;
	}
	void MenuWidgetItem::SetChecked(bool checked)
	{
		if (m_MenuItem)
		{
			m_MenuItem->Check(checked);
		}
	}
}

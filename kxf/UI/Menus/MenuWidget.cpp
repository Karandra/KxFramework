#include "KxfPCH.h"
#include "MenuWidget.h"
#include "MenuWidgetItem.h"
#include "WXUI/Menu.h"
#include "WXUI/MenuItem.h"
#include "../INativeWidget.h"
#include "../Events/MenuWidgetEvent.h"
#include "kxf/Threading/Common.h"
#include "kxf/Drawing/GraphicsRenderer.h"
#include "kxf/Utility/Enumerator.h"
#include <wx/menu.h>
#include <wx/colour.h>
#include <wx/msw/dc.h>
#include <Windows.h>

namespace
{
	std::unordered_map<const wxMenuItem*, std::weak_ptr<kxf::IMenuWidgetItem>> g_MenuItemMap;
}
namespace
{
	constexpr kxf::FlagSet<uint32_t> MapMenuAlignment(kxf::FlagSet<kxf::Alignment> alignment) noexcept
	{
		using namespace kxf;

		FlagSet<uint32_t> nativeAlignment;
		nativeAlignment.Add(TPM_LEFTALIGN, alignment & Alignment::Left);
		nativeAlignment.Add(TPM_RIGHTALIGN, alignment & Alignment::Right);
		nativeAlignment.Add(TPM_CENTERALIGN, alignment & Alignment::CenterHorizontal);
		nativeAlignment.Add(TPM_TOPALIGN, alignment & Alignment::Top);
		nativeAlignment.Add(TPM_BOTTOMALIGN, alignment & Alignment::Bottom);
		nativeAlignment.Add(TPM_VCENTERALIGN, alignment & Alignment::CenterVertical);

		return nativeAlignment;
	}
}

namespace kxf::Private
{
	class MenuWidgetGuard final
	{
		private:
			Widgets::MenuWidget& m_Menu;
			object_ptr<INativeWidget> m_NativeInvokingWidget;
			wxWindow* m_InvokingWindowWX = nullptr;
			Point m_InvokingPosition;

		private:
			void YieldForCommandsOnly() noexcept
			{
				// This right from 'wxWindowMSW::DoPopupMenu', not sure if it's really needed.

				// Peek all WM_COMMANDs (it will always return WM_QUIT too but we don't want to process it here)
				MSG msg = {};
				while (::PeekMessageW(&msg, nullptr, WM_COMMAND, WM_COMMAND, PM_REMOVE))
				{
					if (msg.message == WM_QUIT)
					{
						// If we retrieved a WM_QUIT, insert back into the message queue.
						::PostQuitMessage(0);
						break;
					}

					// Luckily (as we don't have access to wxEventLoopImpl method from here anyhow...)
					// we don't need to pre-process WM_COMMANDs so dispatch it immediately.
					::TranslateMessage(&msg);
					::DispatchMessageW(&msg);
				}
			}

		public:
			MenuWidgetGuard(Widgets::MenuWidget& menu, std::shared_ptr<IWidget> invokingWidget, wxWindow* m_InvokingWindowWX, Point invokingPosition)
				:m_Menu(menu), m_InvokingPosition(invokingPosition)
			{
				// Save invocation data
				if (invokingWidget)
				{
					invokingWidget->QueryInterface(m_NativeInvokingWidget);
				}
				m_Menu.m_InvokingWidget = invokingWidget;
				m_Menu.m_InvokingThread = Threading::GetCurrentThreadID();
				m_Menu.m_InvokingPosition = invokingPosition;
				m_Menu.m_Menu->SetInvokingWindow(m_InvokingWindowWX);

				// Send open event
				MenuWidgetEvent event(m_Menu, std::move(invokingWidget));
				event.SetPopupPosition(invokingPosition);
				m_Menu.ProcessEvent(event, MenuWidgetEvent::EvtOpen);

				// Bring invoking window to the front
				if (m_NativeInvokingWidget)
				{
					m_NativeInvokingWidget->SetForegroundWindow();
				}
			}
			~MenuWidgetGuard()
			{
				// Send close event
				MenuWidgetEvent event(m_Menu, m_Menu.m_InvokingWidget);
				event.SetPopupPosition(m_InvokingPosition);
				m_Menu.ProcessEvent(event, MenuWidgetEvent::EvtClose);

				// And reset invocation data
				if (m_NativeInvokingWidget)
				{
					m_NativeInvokingWidget->PostMessage(WM_NULL);
				}
				m_Menu.m_InvokingThread = 0;
				m_Menu.m_InvokingWidget = nullptr;
				m_Menu.m_InvokingPosition = Point::UnspecifiedPosition();
				m_Menu.m_Menu->SetInvokingWindow(nullptr);

				if (Threading::IsMainThread())
				{
					YieldForCommandsOnly();
				}
			}
	};
}

namespace kxf::Widgets
{
	// MenuWidget
	void MenuWidget::AssociateWXMenuItem(wxMenuItem& wx, IMenuWidgetItem& item) noexcept
	{
		g_MenuItemMap.insert_or_assign(&wx, item.LockReference());
	}
	void MenuWidget::DissociateWXMenuItem(wxMenuItem& wx) noexcept
	{
		g_MenuItemMap.erase(&wx);
	}
	std::shared_ptr<IMenuWidgetItem> MenuWidget::FindByWXMenuItem(const wxMenuItem& wx) noexcept
	{
		auto it = g_MenuItemMap.find(&wx);
		if (it != g_MenuItemMap.end())
		{
			return it->second.lock();
		}
		return nullptr;
	}

	bool MenuWidget::HandleMessage(intptr_t& result, uint32_t msg, intptr_t wParam, intptr_t lParam)
	{
		switch (msg)
		{
			case WM_MENUSELECT:
			{
				const auto indexOrID = LOWORD(wParam);
				const FlagSet flags = HIWORD(wParam);
				const auto handle = reinterpret_cast<HMENU>(lParam);

				wxMenu* menuWX = m_Menu.get();
				wxMenuItem* itemWX = flags.Contains(MF_POPUP) ? m_Menu->FindItemByPosition(indexOrID) : m_Menu->FindItem(*WXUI::Menu::WinIDToWx(indexOrID), &menuWX);

				if (itemWX && (flags.Contains(MF_HILITE) || flags.Contains(MF_MOUSESELECT)))
				{
					object_ptr<IMenuWidget> menuWidget = RTTI::assume_non_owned(*this);
					if (auto item = FindByWXMenuItem(*itemWX))
					{
						// Try to get an actual sub-menu object for the event
						if (auto widget = Private::FindByWXObject(*menuWX))
						{
							widget->QueryInterface(menuWidget);
						}

						MenuWidgetEvent event(*menuWidget, *item, m_InvokingWidget);
						event.SetPopupPosition(m_InvokingPosition);
						item->ProcessEvent(event, MenuWidgetEvent::EvtHover, ProcessEventFlag::HandleExceptions);
					}

					result = TRUE;
					return true;
				}
				break;
			}
			case WM_MENUCOMMAND:
			{
				const auto index = static_cast<uint32_t>(wParam);
				const auto handle = reinterpret_cast<HMENU>(lParam);

				if (wxMenuItem* itemWX = m_Menu->FindItemByPosition(index))
				{
					if (auto item = FindByWXMenuItem(*itemWX))
					{
						MenuWidgetEvent event(*this, *item, m_InvokingWidget);
						event.SetPopupPosition(m_InvokingPosition);
						item->ProcessEvent(event, MenuWidgetEvent::EvtSelect, ProcessEventFlag::HandleExceptions);
					}

					result = TRUE;
					return true;
				}
				break;
			}
			case WM_MEASUREITEM:
			{
				auto id = static_cast<int>(wParam);
				auto measureItem = reinterpret_cast<MEASUREITEMSTRUCT*>(lParam);
				if (id == 0 && measureItem->CtlType == ODT_MENU)
				{
					if (auto menuItem = reinterpret_cast<wxMenuItem*>(measureItem->itemData))
					{
						Geometry::BasicSize<size_t> size;
						bool handled = menuItem->OnMeasureItem(&size.Width(), &size.Height());

						measureItem->itemWidth = static_cast<uint32_t>(size.GetWidth());
						measureItem->itemHeight = static_cast<uint32_t>(size.GetHeight());

						result = TRUE;
						return handled;
					}
				}
				break;
			}
			case WM_DRAWITEM:
			{
				auto id = static_cast<int>(wParam);
				auto drawItem = reinterpret_cast<DRAWITEMSTRUCT*>(lParam);
				if (id == 0 && drawItem->CtlType == ODT_MENU)
				{
					if (auto menuItem = reinterpret_cast<wxMenuItem*>(drawItem->itemData))
					{
						// Using 'wxDCTemp' to prevent the DC from being released
						wxDCTemp dc(static_cast<HDC>(drawItem->hDC));
						wxRect rect(drawItem->rcItem.left, drawItem->rcItem.top, drawItem->rcItem.right - drawItem->rcItem.left, drawItem->rcItem.bottom - drawItem->rcItem.top);

						if (menuItem->OnDrawItem(dc, rect, static_cast<wxOwnerDrawn::wxODAction>(drawItem->itemAction), static_cast<wxOwnerDrawn::wxODStatus>(drawItem->itemState)))
						{
							result = TRUE;
							return true;
						}
					}
				}
				break;
			}
			case WM_NEXTMENU:
			case WM_MENUCHAR:
			case WM_MENUDRAG:
			case WM_MENURBUTTONUP:
			case WM_INITMENUPOPUP:
			case WM_ENTERMENULOOP:
			case WM_EXITMENULOOP:
			{
				// TODO: Handle those messages in the future
				break;
			}
		};
		return false;
	}
	bool MenuWidget::DoShow(Point screenPos, FlagSet<Alignment> alignment, std::shared_ptr<IWidget> invokingWidget)
	{
		if (!m_NativeWindow)
		{
			m_NativeWindow.Create([&](intptr_t& result, uint32_t msg, intptr_t wParam, intptr_t lParam)
			{
				return HandleMessage(result, msg, wParam, lParam);
			});
			if (!m_NativeWindow)
			{
				return false;
			}
		}

		auto flags = MapMenuAlignment(alignment);
		flags.Add(TPM_LEFTBUTTON);
		flags.Add(TPM_RECURSE);

		Private::MenuWidgetGuard guard(*this, std::move(invokingWidget), m_NativeWindow.GetWxWindow(), screenPos);
		return ::TrackPopupMenuEx(m_Menu->GetHMenu(), *flags, screenPos.GetX(), screenPos.GetY(), static_cast<HWND>(m_NativeWindow.GetHandle()), nullptr);
	}

	bool MenuWidget::DoDestroyWidget(bool releaseWX)
	{
		if (m_Menu)
		{
			// Move the object here to prevent calling this function again upon destructing the wxMenu object.
			auto menu = std::move(m_Menu);

			m_NativeWindow.Destroy();
			Private::DissociateWXObject(*menu);

			if (releaseWX || m_IsAttached)
			{
				static_cast<void>(menu.release());
			}
			return true;
		}
		return false;
	}
	void MenuWidget::OnWXMenuDestroyed()
	{
		// Underlying wxMenu is being destroyed, that means we need
		// to release the pointer to it so we don't double delete it.
		DoDestroyWidget(true);
	}

	MenuWidget::MenuWidget()
		:m_EventHandlerStack(m_EvtHandler), m_Menu(std::make_unique<WXUI::Menu>(*this))
	{
		Private::AssociateWXObject(*m_Menu, *this);
	}
	MenuWidget::~MenuWidget()
	{
		DoDestroyWidget();
	}

	// Native interface
	void* MenuWidget::GetHandle() const
	{
		return m_Menu ? m_Menu->GetHMenu() : nullptr;
	}

	// Lifetime management
	bool MenuWidget::IsWidgetAlive() const
	{
		return m_Menu != nullptr;
	}
	bool MenuWidget::CreateWidget(IWidget* parent, const String& text, Point pos, Size size)
	{
		if (!m_Menu)
		{
			m_Menu = std::make_unique<WXUI::Menu>(*this);
			m_Menu->SetTitle(text);

			m_Label = text;
			m_PopupPosition = pos;
			m_MaxSize = size;
			if (parent)
			{
				m_ParentWidget = parent->LockReference();
			}

			Private::AssociateWXObject(*m_Menu, *this);
			return true;
		}
		return false;
	}
	bool MenuWidget::CloseWidget(bool force)
	{
		if (m_Menu && m_InvokingThread == Threading::GetCurrentThreadID())
		{
			return ::EndMenu();
		}
		return false;
	}
	bool MenuWidget::DestroyWidget()
	{
		return DoDestroyWidget();
	}

	// Child management functions
	void MenuWidget::AddChildWidget(IWidget& widget)
	{
		object_ptr<IMenuWidget> menuWidget;
		if (m_Menu && widget.QueryInterface(menuWidget))
		{
			MenuWidget::InsertMenu(*menuWidget);
		}
	}
	void MenuWidget::RemoveChildWidget(const IWidget& widget)
	{
		if (m_Menu && widget.QueryInterface<IMenuWidget>())
		{
			void* handle = widget.GetHandle();
			for (auto& item: m_Menu->GetMenuItems())
			{
				wxMenu* menu = item->GetSubMenu();
				if (menu && menu->GetHMenu() == handle)
				{
					m_Menu->Destroy(item);
				}
			}
		}
	}
	void MenuWidget::DestroyChildWidgets()
	{
		if (m_Menu)
		{
			for (auto& item: m_Menu->GetMenuItems())
			{
				// Destroy only the sub-menus (as they're widgets unlike items)
				if (item->GetSubMenu())
				{
					m_Menu->Destroy(item);
				}
			}
		}
	}

	std::shared_ptr<IWidget> MenuWidget::FindChildWidgetByID(WidgetID id) const
	{
		return nullptr;
	}
	std::shared_ptr<IWidget> MenuWidget::FindChildWidgetByName(const String& widgetName) const
	{
		return nullptr;
	}
	Enumerator<std::shared_ptr<IWidget>> MenuWidget::EnumChildWidgets() const
	{
		return {};
	}

	String MenuWidget::GetWidgetText(FlagSet<WidgetTextFlag> flags) const
	{
		if (flags.Contains(WidgetTextFlag::WithMnemonics))
		{
			return m_Label;
		}
		return RemoveMnemonics(m_Label);
	}
	void MenuWidget::SetWidgetText(const String& widgetText, FlagSet<WidgetTextFlag> flags)
	{
		m_Label = widgetText;
		if (m_Menu)
		{
			m_Menu->SetTitle(widgetText);
		}
	}

	// IMenuWidget
	std::shared_ptr<IMenuWidgetItem> MenuWidget::InsertItem(IMenuWidgetItem& item, size_t index)
	{
		if (m_Menu)
		{
			if (auto menuItem = item.QueryInterface<MenuWidgetItem>())
			{
				menuItem->m_OwningMenu = m_WidgetReference;
				m_Menu->Insert(std::clamp(index, 0_uz, m_Menu->GetMenuItemCount()), menuItem->m_MenuItem.get());

				return menuItem->LockReference();
			}
		}
		return nullptr;
	}
	std::shared_ptr<IMenuWidgetItem> MenuWidget::InsertMenu(IMenuWidget& subMenu, const String& label, WidgetID id, size_t index)
	{
		object_ptr<MenuWidget> menuWidget;
		if (m_Menu && subMenu.IsWidgetAlive() && subMenu.QueryInterface(menuWidget) && !menuWidget->m_IsAttached)
		{
			menuWidget->m_IsAttached = true;

			// Create item
			auto item = std::make_shared<MenuWidgetItem>();
			item->m_MenuItem = std::make_unique<WXUI::MenuItem>(item, menuWidget->LockMenuReference(), *menuWidget->m_Menu);
			item->m_OwningMenu = m_WidgetReference;
			item->SaveReference(item);
			item->DoCreateWidget();

			item->SetLabel(!label.IsEmpty() ? label : menuWidget->GetLabel(), WidgetTextFlag::WithMnemonics);
			item->SetItemID(!id.IsNone() ? id : menuWidget->GetWidgetID());
			item->SetDescription(menuWidget->GetDescription());

			// Insert the menu
			m_Menu->Insert(std::clamp(index, 0_uz, m_Menu->GetMenuItemCount()), item->m_MenuItem.get());
			return item;
		}
		return nullptr;
	}

	std::shared_ptr<IMenuWidgetItem> MenuWidget::CreateItem(const String& label, MenuWidgetItemType type, WidgetID id)
	{
		if (m_Menu && type != MenuWidgetItemType::None)
		{
			auto item = std::make_shared<MenuWidgetItem>();
			switch (type)
			{
				case MenuWidgetItemType::Separator:
				{
					item->m_MenuItem = std::make_unique<WXUI::MenuItem>(item, wxITEM_SEPARATOR);
					break;
				}
				case MenuWidgetItemType::CheckItem:
				{
					item->m_MenuItem = std::make_unique<WXUI::MenuItem>(item, wxITEM_CHECK);
					break;
				}
				case MenuWidgetItemType::RadioItem:
				{
					item->m_MenuItem = std::make_unique<WXUI::MenuItem>(item, wxITEM_RADIO);
					break;
				}
				default:
				{
					item->m_MenuItem = std::make_unique<WXUI::MenuItem>(item, wxITEM_NORMAL);
					break;
				}
			};

			item->SaveReference(item);
			item->DoCreateWidget();
			item->SetLabel(label);
			item->SetItemID(id);

			return item;
		}
		return nullptr;
	}
	std::shared_ptr<IMenuWidgetItem> MenuWidget::GetDefaultItem() const
	{
		if (m_Menu)
		{
			auto id = ::GetMenuDefaultItem(m_Menu->GetHMenu(), FALSE, GMDI_GOINTOPOPUPS|GMDI_USEDISABLED);
			if (id != std::numeric_limits<decltype(id)>::max())
			{
				if (auto item = m_Menu->FindItem(*WXUI::Menu::WinIDToWx(id)))
				{
					return FindByWXMenuItem(*item);
				}
			}
		}
		return nullptr;
	}
	Enumerator<std::shared_ptr<IMenuWidgetItem>> MenuWidget::EnumMenuItems() const
	{
		if (m_Menu)
		{
			return Utility::EnumerateIterableContainer<std::shared_ptr<IMenuWidgetItem>>(m_Menu->GetMenuItems(), [&](const wxMenuItem* item)
			{
				return FindByWXMenuItem(*item);
			});
		}
		return {};
	}

	void MenuWidget::Show(Point pos, FlagSet<Alignment> alignment)
	{
		if (m_Menu)
		{
			if (!pos.IsFullySpecified())
			{
				if (m_PopupPosition.IsFullySpecified())
				{
					pos = m_ParentWidget ? m_ParentWidget->ClientToScreen(m_PopupPosition) : m_PopupPosition;
				}
				else
				{
					pos = GetMouseCoordinates();
				}
			}
			DoShow(pos, alignment, m_ParentWidget);
		}
	}
	void MenuWidget::ShowAt(const IWidget& widget, Point pos, FlagSet<Alignment> alignment)
	{
		if (m_Menu)
		{
			if (!pos.IsFullySpecified())
			{
				pos = widget.GetPosition();
				if (auto parent = widget.GetParentWidget())
				{
					pos = parent->ClientToScreen(pos);
				}
			}
			DoShow(pos, alignment, widget.LockReference());
		}
	}
	void MenuWidget::ShowWithOffset(const IWidget& widget, int offset, FlagSet<Alignment> alignment)
	{
		if (m_Menu)
		{
			Size size = widget.GetSize();
			Point pos = {0, size.GetHeight() + offset};

			if (alignment.Contains(Alignment::Right))
			{
				pos.X() = size.GetWidth();
			}
			else if (alignment.Contains(Alignment::Bottom))
			{
				pos.Y() = -offset;
			}

			DoShow(widget.ClientToScreen(pos), alignment, widget.LockReference());
		}
	}
}

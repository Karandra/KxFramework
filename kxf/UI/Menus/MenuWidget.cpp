#include "KxfPCH.h"
#include "MenuWidget.h"
#include "../INativeWidget.h"
#include "../Events/MenuWidgetEvent.h"
#include "kxf/Threading/Common.h"
#include "kxf/Drawing/GraphicsRenderer.h"
#include "kxf/Utility/Enumerator.h"
#include <wx/menu.h>
#include <wx/colour.h>
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

	constexpr uint16_t WxIDToWin(int menuWxID) noexcept
	{
		return static_cast<uint32_t>(menuWxID);
	}
	constexpr kxf::WidgetID WinIDToWx(uint16_t menuWinID) noexcept
	{
		return static_cast<int16_t>(menuWinID);
	}
	constexpr kxf::WidgetID WinMenuRetToWx(int id) noexcept
	{
		if (id != 0)
		{
			return WinIDToWx(static_cast<uint16_t>(id));
		}
		return {};
	}
}

namespace kxf::Private
{
	class MenuWidgetGuard final
	{
		private:
			Widgets::MenuWidget& m_Menu;
			object_ptr<INativeWidget> m_NativeInvokingWidget;
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
			MenuWidgetGuard(Widgets::MenuWidget& menu, std::shared_ptr<IWidget> invokingWidget, Point invokingPosition)
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
				wxMenuItem* itemWX = flags.Contains(MF_POPUP) ? m_Menu->FindItemByPosition(indexOrID) : m_Menu->FindItem(*WinIDToWx(indexOrID), &menuWX);

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
			if (m_NativeWindow.IsNull())
			{
				return false;
			}
		}

		auto flags = MapMenuAlignment(alignment);
		flags.Add(TPM_LEFTBUTTON);
		flags.Add(TPM_RECURSE);

		Private::MenuWidgetGuard guard(*this, std::move(invokingWidget), screenPos);
		return ::TrackPopupMenuEx(m_Menu->GetHMenu(), *flags, screenPos.GetX(), screenPos.GetY(), static_cast<HWND>(m_NativeWindow.GetHandle()), nullptr);
	}

	MenuWidget::MenuWidget()
		:m_EventHandlerStack(m_EvtHandler), m_Menu(std::make_unique<wxMenu>())
	{
	}
	MenuWidget::~MenuWidget()
	{
		MenuWidget::DestroyWidget();
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
			m_Menu = std::make_unique<wxMenu>();
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
		if (m_Menu)
		{
			m_NativeWindow.Destroy();
			Private::DissociateWXObject(*m_Menu);
			m_Menu = nullptr;

			return true;
		}
		return false;
	}

	// Child management functions
	void MenuWidget::AddChildWidget(IWidget& widget)
	{
	}
	void MenuWidget::RemoveChildWidget(const IWidget& widget)
	{
	}
	void MenuWidget::DestroyChildWidgets()
	{
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
		return nullptr;
	}
	std::shared_ptr<IMenuWidgetItem> MenuWidget::InsertMenu(IMenuWidget& subMenu, size_t index)
	{
		return nullptr;
	}
	std::shared_ptr<IMenuWidgetItem> MenuWidget::InsertSeparator(size_t index)
	{
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

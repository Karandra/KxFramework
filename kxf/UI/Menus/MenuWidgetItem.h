#pragma once
#include "Common.h"
#include "../IMenuWidget.h"
#include "../IMenuWidgetItem.h"
#include "kxf/General/Enumerator.h"
#include "kxf/EventSystem/BasicEvtHandler.h"
#include "kxf/EventSystem/EventHandlerStack.h"
class wxMenu;
class wxMenuItem;

namespace kxf::WXUI
{
	class MenuItem;
}

namespace kxf::Widgets
{
	class MenuWidgetItem: public RTTI::DynamicImplementation<MenuWidgetItem, BasicEvtHandler<MenuWidgetItem, IMenuWidgetItem>>
	{
		KxRTTI_DeclareIID(MenuWidgetItem, {0x56983711, 0xc1a4, 0x48ec, {0x97, 0xfd, 0x61, 0x4b, 0x53, 0x2f, 0xc0, 0x44}});

		friend class MenuWidget;
		friend class WXUI::MenuItem;
		friend class BasicEvtHandler<MenuWidgetItem, IMenuWidgetItem>;

		private:
			EvtHandler m_EvtHandler;
			std::unique_ptr<WXUI::MenuItem> m_MenuItem;
			std::weak_ptr<IMenuWidget> m_OwningMenu;
			std::weak_ptr<IMenuWidgetItem> m_WidgetReference;

			// Options
			WidgetID m_ItemID;

		private:
			EvtHandler& GetThisEvtHandler() noexcept
			{
				return m_EvtHandler;
			}
			const EvtHandler& GetThisEvtHandler() const noexcept
			{
				return m_EvtHandler;
			}

			EvtHandler& GetTopEvtHandler() noexcept
			{
				return m_EvtHandler;
			}
			const EvtHandler& GetTopEvtHandler() const noexcept
			{
				return m_EvtHandler;
			}

			void DoCreateWidget();
			bool DoDestroyWidget(bool releaseWX = false);
			void OnWXMenuDestroyed();

		protected:
			// IMenuWidgetItem
			void SaveReference(std::weak_ptr<IMenuWidgetItem> ref) noexcept override
			{
				m_WidgetReference = std::move(ref);
			}

		public:
			MenuWidgetItem();
			MenuWidgetItem(const MenuWidgetItem&) = delete;
			~MenuWidgetItem();

			// --- IMenuWidgetItem ---
		public:
			// Lifetime management
			std::shared_ptr<IMenuWidgetItem> LockReference() const override
			{
				return m_WidgetReference.lock();
			}

			// General
			std::shared_ptr<IMenuWidget> GetOwningMenu() const override
			{
				return m_OwningMenu.lock();
			}

			std::shared_ptr<IMenuWidget> GetSubMenu() const override;
			void SetSubMenu(IMenuWidget& subMenu) override;

			bool IsDefaultItem() const override;
			void SetDefaultItem() override;

			// Properties
			MenuWidgetItemType GetItemType() const override;

			String GetLabel(FlagSet<WidgetTextFlag> flags = {}) const override;
			void SetLabel(const String& label, FlagSet<WidgetTextFlag> flags = {}) override;

			String GetDescription() const override;
			void SetDescription(const String& description) override;

			WidgetID GetItemID() const override;
			void SetItemID(WidgetID id) override;

			BitmapImage GetItemIcon() const override;
			void SetItemIcon(const BitmapImage& icon) override;

			bool IsEnabled() const override;
			void SetEnabled(bool enabled = true) override;

			bool IsChecked() const override;
			void SetChecked(bool checked = true) override;

		public:
			MenuWidgetItem& operator=(const MenuWidgetItem&) = delete;
	};
}

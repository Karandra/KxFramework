#pragma once
#include "Common.h"
#include "IWidget.h"
#include "kxf/EventSystem/IEvtHandler.h"
#include "kxf/EventSystem/IEvent.h"

namespace kxf
{
	class IMenuWidget;
	class BitmapImage;

	enum class MenuWidgetItemType
	{
		None = -1,

		Regular,
		SubMenu,
		Separator,
		CheckItem,
		RadioItem
	};
}

namespace kxf
{
	class KX_API IMenuWidgetItem: public RTTI::ExtendInterface<IMenuWidgetItem, IEvtHandler>
	{
		KxRTTI_DeclareIID(IMenuWidgetItem, {0xb1e0d965, 0x3eec, 0x42d8, {0xa4, 0x4e, 0xd9, 0xce, 0xd7, 0xb7, 0x93, 0x3e}});

		protected:
			virtual void SaveReference(std::weak_ptr<IMenuWidgetItem> ref) = 0;

		public:
			virtual ~IMenuWidgetItem() = default;

		public:
			// Lifetime management
			virtual std::shared_ptr<IMenuWidgetItem> LockReference() const = 0;

			// General
			virtual std::shared_ptr<IMenuWidget> GetOwningMenu() const = 0;

			virtual std::shared_ptr<IMenuWidget> GetSubMenu() const = 0;
			virtual void SetSubMenu(IMenuWidget& subMenu) = 0;

			virtual bool IsDefaultItem() const = 0;
			virtual void SetDefaultItem() = 0;

			// Properties
			virtual MenuWidgetItemType GetItemType() const = 0;
			bool IsRegularItem() const
			{
				return GetItemType() == MenuWidgetItemType::Separator;
			}
			bool IsSeparatorItem() const
			{
				return GetItemType() == MenuWidgetItemType::Separator;
			}
			bool IsCheckItem() const
			{
				return GetItemType() == MenuWidgetItemType::CheckItem;
			}
			bool IsRadioItem() const
			{
				return GetItemType() == MenuWidgetItemType::RadioItem;
			}
			bool IsSubMenuItem() const
			{
				return GetItemType() == MenuWidgetItemType::SubMenu;
			}

			virtual String GetLabel(FlagSet<WidgetTextFlag> flags = {}) const = 0;
			virtual void SetLabel(const String& label, FlagSet<WidgetTextFlag> flags = {}) = 0;

			virtual String GetDescription() const = 0;
			virtual void SetDescription(const String& description) = 0;

			virtual WidgetID GetItemID() const = 0;
			virtual void SetItemID(WidgetID id) = 0;

			virtual BitmapImage GetItemIcon() const = 0;
			virtual void SetItemIcon(const BitmapImage& icon) = 0;

			virtual bool IsEnabled() const = 0;
			virtual void SetEnabled(bool enabled = true) = 0;

			virtual bool IsChecked() const = 0;
			virtual void SetChecked(bool checked = true) = 0;
	};
}

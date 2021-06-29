#pragma once
#include "Common.h"
#include "kxf/EventSystem/IEvtHandler.h"
#include "IWidget.h"

namespace kxf
{
	class IMenuWidgetItem;
}

namespace kxf
{
	class KX_API IMenuWidget: public RTTI::ExtendInterface<IMenuWidget, IWidget>
	{
		KxRTTI_DeclareIID(IMenuWidget, {0x1f082719, 0x1c5e, 0x4253, {0x87, 0xb2, 0x5a, 0x7a, 0xb, 0xb8, 0xf2, 0x6e}});

		public:
			static constexpr size_t npos = std::numeric_limits<size_t>::max();

		public:
			virtual ~IMenuWidget() = default;

		public:
			virtual String GetLabel(FlagSet<WidgetTextFlag> flags = {}) const = 0;
			virtual void SetLabel(const String& label, FlagSet<WidgetTextFlag> flags = {}) = 0;

			virtual String GetDescription() const = 0;
			virtual void SetDescription(const String& description) = 0;

			virtual std::shared_ptr<IMenuWidgetItem> InsertItem(IMenuWidgetItem& item, size_t index = npos) = 0;
			virtual std::shared_ptr<IMenuWidgetItem> InsertMenu(IMenuWidget& subMenu, size_t index = npos) = 0;
			virtual std::shared_ptr<IMenuWidgetItem> InsertSeparator(size_t index = npos) = 0;
			virtual Enumerator<std::shared_ptr<IMenuWidgetItem>> EnumMenuItems() const = 0;

			virtual void Show(Point pos = Point::UnspecifiedPosition(), FlagSet<Alignment> alignment = {}) = 0;
			virtual void ShowAt(const IWidget& widget, Point pos = Point::UnspecifiedPosition(), FlagSet<Alignment> alignment = {}) = 0;
			virtual void ShowWithOffset(const IWidget& widget, int offset = 1, FlagSet<Alignment> alignment = {}) = 0;
	};
}

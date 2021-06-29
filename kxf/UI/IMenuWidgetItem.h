#pragma once
#include "Common.h"
#include "IWidget.h"
#include "kxf/EventSystem/IEvtHandler.h"
#include "kxf/EventSystem/IEvent.h"

namespace kxf
{
	class IMenuWidget;
}

namespace kxf
{
	class KX_API IMenuWidgetItem: public RTTI::ExtendInterface<IMenuWidgetItem, IEvtHandler>
	{
		KxRTTI_DeclareIID(IMenuWidgetItem, {0xb1e0d965, 0x3eec, 0x42d8, {0xa4, 0x4e, 0xd9, 0xce, 0xd7, 0xb7, 0x93, 0x3e}});

		public:
			virtual ~IMenuWidgetItem() = default;

		public:
			virtual std::shared_ptr<IMenuWidgetItem> LockReference() const = 0;

		public:
			virtual std::shared_ptr<IMenuWidget> GetOwningMenu() const = 0;

			virtual String GetLabel(FlagSet<WidgetTextFlag> flags = {}) const = 0;
			virtual void SetLabel(const String& label, FlagSet<WidgetTextFlag> flags = {}) = 0;

			virtual String GetDescription() const = 0;
			virtual void SetDescription(const String& description) = 0;

			virtual WidgetID GetItemID() const = 0;
			virtual void SetItemID(WidgetID id) = 0;
	};
}

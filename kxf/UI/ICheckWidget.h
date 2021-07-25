#pragma once
#include "Common.h"
#include "IWidget.h"

namespace kxf
{
	enum class CheckWidgetValue
	{
		Checked = 1,
		Unchecked = 0,
		Indeterminate = -1
	};
}

namespace kxf
{
	class KX_API ICheckWidget: public RTTI::ExtendInterface<ICheckWidget, IWidget>
	{
		KxRTTI_DeclareIID(ICheckWidget, {0xffcee75f, 0x809d, 0x417f, {0x96, 0x6d, 0xc4, 0x44, 0x83, 0xaa, 0xc8, 0x6e}});

		public:
			virtual bool IsThirdStateSelectable() const = 0;
			virtual void SetThirdStateSelectable(bool isSelectable = true) = 0;

			virtual CheckWidgetValue GetValue() const = 0;
			virtual void SetValue(CheckWidgetValue value) = 0;

			bool IsChecked() const
			{
				return GetValue() == CheckWidgetValue::Checked;
			}
			void SetChecked(bool checked = true)
			{
				SetValue(checked ? CheckWidgetValue::Checked : CheckWidgetValue::Unchecked);
			}
	};
}

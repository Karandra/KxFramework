#pragma once
#include "Common.h"
#include "IWidget.h"

namespace kxf
{
	class KX_API IStaticLabel: public RTTI::ExtendInterface<IStaticLabel, IWidget>
	{
		KxRTTI_DeclareIID(IStaticLabel, {0x205021cb, 0x1bd9, 0x4014, {0xb2, 0x62, 0xdf, 0xbb, 0xc7, 0xa6, 0xa2, 0x99}});

		public:
			virtual String GetLabel(FlagSet<WidgetTextFlag> flags = {}) const = 0;
			virtual void SetLabel(const String& label, FlagSet<WidgetTextFlag> flags = {}) = 0;

			virtual BitmapImage GetIcon() const = 0;
			virtual void SetIcon(const BitmapImage& icon, FlagSet<Direction> direction = {}) = 0;
			virtual void SetStdIcon(FlagSet<StdIcon> stdIcon, FlagSet<Direction> direction = {}) = 0;
	};
}

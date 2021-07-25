#pragma once
#include "Common.h"
#include "IWidget.h"

namespace kxf
{
	class KX_API IStaticLabel: public RTTI::ExtendInterface<IStaticLabel, IWidget>
	{
		KxRTTI_DeclareIID(ICheckWidget, {0x205021cb, 0x1bd9, 0x4014, {0xb2, 0x62, 0xdf, 0xbb, 0xc7, 0xa6, 0xa2, 0x99}});

		public:
			virtual BitmapImage GetLabelIcon() const = 0;
			virtual void SetLabelIcon(const BitmapImage& icon, FlagSet<Direction> direction = {}) = 0;
			virtual void SetStdLabelIcon(FlagSet<StdIcon> stdIcon, FlagSet<Direction> direction = {}) = 0;
	};
}

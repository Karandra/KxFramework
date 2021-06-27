#pragma once
#include "Common.h"
#include "ITopLevelWidget.h"

namespace kxf
{
	class KX_API IDalogWidget: public RTTI::ExtendInterface<IDalogWidget, ITopLevelWidget>
	{
		KxRTTI_DeclareIID(IDalogWidget, {0xa6bd3c33, 0x4d1e, 0x4a13, {0x8a, 0x16, 0xb6, 0xc9, 0x26, 0x9e, 0xb0, 0xc1}});

		public:
			virtual ~IDalogWidget() = default;

		public:
			virtual bool IsModal() const = 0;
			virtual void EndModal(int retCode) = 0;
			virtual int ShowModal() = 0;
			virtual int GetReturnCode() const = 0;

			virtual int GetAffirmativeID() const = 0;
			virtual void SetAffirmativeID(int id) = 0;

			virtual int GetEscapeID() const = 0;
			virtual void SetEscapeID(int id) = 0;
	};
}

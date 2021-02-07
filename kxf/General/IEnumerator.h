#pragma once
#include "Common.h"
#include "kxf/RTTI/RTTI.h"

namespace kxf
{
	class IEnumerator: public RTTI::Interface<IEnumerator>
	{
		KxRTTI_DeclareIID(IEnumerator, {0x6d4aed72, 0x3c54, 0x4541, {0x9c, 0x2a, 0x57, 0x7a, 0x94, 0x3d, 0x12, 0x73}});

		public:
			virtual bool MoveNext() noexcept = 0;
			virtual void Terminate() noexcept = 0;
			virtual size_t GetCurrentStep() const noexcept = 0;

			virtual bool IsReset() const noexcept = 0;
			virtual void Reset() noexcept = 0;

		public:
			explicit operator bool() const noexcept
			{
				return !IsReset();
			}
			bool operator!() const noexcept
			{
				return IsReset();
			}
	};
}

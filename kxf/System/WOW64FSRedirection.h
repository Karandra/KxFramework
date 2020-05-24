#pragma once
#include "Common.h"

namespace kxf
{
	class KX_API DisableWOW64FSRedirection final
	{
		private:
			void* m_Value = nullptr;

		public:
			DisableWOW64FSRedirection() noexcept;
			~DisableWOW64FSRedirection() noexcept;
	};
}

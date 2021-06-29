#pragma once
#include "kxf/UI/Private/AnonymousNativeWindow.h"

namespace kxf
{
	class UniversallyUniqueID;
	class DefaultRPCExchanger;
}

namespace kxf
{
	class DefaultRPCExchangerWindow final
	{
		private:
			Private::AnonymousNativeWindow m_Window;
			DefaultRPCExchanger& m_Exchanger;

		public:
			DefaultRPCExchangerWindow(DefaultRPCExchanger& exchanger) noexcept
				:m_Exchanger(exchanger)
			{
			}

		public:
			void* GetHandle() const noexcept
			{
				return m_Window.GetHandle();
			}

			bool Create(const UniversallyUniqueID& sessionID);
			bool Destroy() noexcept;
	};
}

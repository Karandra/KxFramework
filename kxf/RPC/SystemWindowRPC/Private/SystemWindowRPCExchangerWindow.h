#pragma once
#include "kxf/UI/Private/AnonymousNativeWindow.h"

namespace kxf
{
	class UniversallyUniqueID;
	class SystemWindowRPCExchanger;
}

namespace kxf
{
	class SystemWindowRPCExchangerWindow final
	{
		private:
			Private::AnonymousNativeWindow m_Window;
			SystemWindowRPCExchanger& m_Exchanger;

		public:
			SystemWindowRPCExchangerWindow(SystemWindowRPCExchanger& exchanger) noexcept
				:m_Exchanger(exchanger)
			{
			}

		public:
			void* GetHandle() const noexcept
			{
				return m_Window.GetHandle();
			}

			bool Create(const String& sessionID);
			bool Destroy() noexcept;
	};
}
